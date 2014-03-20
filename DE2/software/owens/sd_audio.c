//runs mono

#include "sd_audio.h"
#include <stdlib.h>


#define VERBOSE 0
#define V_VERBOSE 0


int p_shift = 0;




void reset_audio_buffer(audio_buffer* ab) {
	ab->start = 0;
	ab->end = 1;
	ab->finished = false;

	ab->sfx_flag = false;
	ab->sfx_count = 0;

	ab->sfx_flag1 = false;
	ab->sfx_count1 = 0;
}

void av_config_setup() {
	alt_up_av_config_dev *av_config = alt_up_av_config_open_dev("/dev/av_config");
		// "/dev/audio_and_video_config"
	//assert(av_config);
	while (!alt_up_av_config_read_ready(av_config));
}

void audio_isr(void* context, unsigned int id) {

	audio_buffer *ab = (audio_buffer*)context;
	int start = ab->start;
//	int end = ab->end;
/*
	if(start < end){
		if( start + 95 >= end)
			alt_up_audio_disable_write_interrupt(ab->audio_dev);
	}else{
		if( (start + 95)%BUFFER_SIZE >= end )
			alt_up_audio_disable_write_interrupt(ab->audio_dev);
	}*/

	audio_write_fifo(ab->audio_dev,(short int*) ab->cir_buffer+start, 95);

	start += 95;
	if( start == BUFFER_SIZE ) start = 0;

	ab->start = start;

	return;
}

//init sd
//call before any SD_card operation
//Input: nothing
//Output: Make SD I/O available to the user
void sd_init() {

	alt_up_sd_card_dev *sd_init = NULL; //initialize the sd_card driver
	sd_init = alt_up_sd_card_open_dev("/dev/sd_card");

	alt_up_sd_card_is_Present();
	alt_up_sd_card_is_FAT16();

//	alt_up_sd_card_is_Present();
//	alt_up_sd_card_is_FAT16();
//
//	alt_up_sd_card_is_Present();
//	alt_up_sd_card_is_FAT16();
}

//tested working
sdf init_read_wav(char *name) {

	sdf music_handle = sd_fopen(name);

	//discard the first 44 bytes of data (not PCM)
	int i;
	short int a;
	if(VERBOSE)printf("fh: %i OffsetData: ", music_handle);
	for(i = 0; i < 44; i++){
		a = sd_readbyte(music_handle);
		if(VERBOSE)printf("%X ", a);
	}

	if(VERBOSE)printf("\n");
	return music_handle;
}

void refill_buffer(audio_buffer *ab, char* music_to_loop) {
    fill_buffer(ab);
    if(ab->finished){
    	sd_fclose(ab->wav);
    	ab->wav = init_read_wav(music_to_loop);
    	ab->finished = false;
    }
}

//this modifies end
void fill_buffer(audio_buffer *ab) {

	sdf handle = ab->wav;

	short int *cir_buffer = ab->cir_buffer;
	bool finished = ab->finished;
	int end = ab->end;
	int start = ab->start;

	short int temp_data1;
	short int temp_data2;

	int avail_to_fill = start > end ? start - end : start + BUFFER_SIZE - end;
	avail_to_fill--; //start != end

	int i;

	for(i = 0; i < avail_to_fill; i++) {
		//read byte
		temp_data1 = sd_readbyte(handle);
		if( temp_data1 < 0 ) { //reached EOF
			finished = true;
			printf("THIS IS NOT SUPPOSE TO HAPPEN (so early) i = %i !\n", i);
			printf("Avail to fill: %i\n", avail_to_fill);
			break;
		}
		temp_data2 = sd_readbyte(handle);

		if(ab->sfx_flag || ab->sfx_flag1){
			int t = 0;

			if(ab->sfx_flag) {
				t += (short int)((temp_data2 << 8) | temp_data1)
					+ (short int)(ab->sfx_ptr[ab->sfx_count]);
				ab->sfx_count++;
				if(ab->sfx_count == ab->sfx_size){
					ab->sfx_flag = false;
					ab->sfx_count = 0;
				}
			}

			if(ab->sfx_flag1) {
				t += (short int)((temp_data2 << 8) | temp_data1)
					+ (short int)(ab->sfx_ptr1[ab->sfx_count1]);
				ab->sfx_count1++;
				if(ab->sfx_count1 == ab->sfx_size1){
					ab->sfx_flag1 = false;
					ab->sfx_count1 = 0;
				}
			}

			if( t > 32767 )
				cir_buffer[end] = 0x7FFF; //cap it
			else if( t < -32768 )
				cir_buffer[end] = 0x8000; //lower cap it
			else
				cir_buffer[end] = t;


		}else{
			cir_buffer[end]   = (short int)((temp_data2 << 8) | temp_data1);
		}

		end++;

		if( end == BUFFER_SIZE ) end = 0; //reached end of buffer
	}

	ab->end = end;
	ab->finished = finished;
}

//opens a file at location name
//eg. name = "sd.txt" opens sd.txt in root
//	  name = "/dir/w.txt"
//returns the file handle
sdf sd_fopen(char* name){
	return alt_up_sd_card_fopen(name,false);
}

//closes a file
//takes a file handle
void sd_fclose(sdf handle) {
	alt_up_sd_card_fclose(handle);
}

short int sd_readbyte(sdf handle) {
	return alt_up_sd_card_read(handle);
}

//len in multiple of 5's
void audio_write_fifo(alt_up_audio_dev *audio, short int *buf, int len)
{
	int count = 0;
	while ( count < len ) {
		IOWR_ALT_UP_AUDIO_LEFTDATA(audio->base, buf[count]);
		IOWR_ALT_UP_AUDIO_LEFTDATA(audio->base, buf[count+1]);
		IOWR_ALT_UP_AUDIO_LEFTDATA(audio->base, buf[count+2]);
		IOWR_ALT_UP_AUDIO_LEFTDATA(audio->base, buf[count+3]);
		IOWR_ALT_UP_AUDIO_LEFTDATA(audio->base, buf[count+4]);

		IOWR_ALT_UP_AUDIO_RIGHTDATA(audio->base, buf[count]);
		IOWR_ALT_UP_AUDIO_RIGHTDATA(audio->base, buf[count+1]);
		IOWR_ALT_UP_AUDIO_RIGHTDATA(audio->base, buf[count+2]);
		IOWR_ALT_UP_AUDIO_RIGHTDATA(audio->base, buf[count+3]);
		IOWR_ALT_UP_AUDIO_RIGHTDATA(audio->base, buf[count+4]);

		count += 5;
	}
}

//loads sfx
void preload_sfx(char* name, audio_buffer *ab) {

	sdf sfx = init_read_wav(name);

	short int temp_data1, temp_data2;
	int i;
	int size = 0;
	short int max_height = 0;
	short int max_depth = 0;

	//checks length
	while(1)
	{
		temp_data1 = sd_readbyte(sfx);
		if( temp_data1 < 0 ) { //reached EOF
			break;
		}
		sd_readbyte(sfx);
		size++;
	}

	sd_fclose(sfx);

	ab->sfx_ptr = (short int*) malloc(size*sizeof(short int));

	sfx = init_read_wav(name);
	if(VERBOSE)printf("Size: %i word(2bytes)\n",size);

	for(i = 0; i < size; i++) {
		temp_data1 = sd_readbyte(sfx);
		temp_data2 = sd_readbyte(sfx);

		ab->sfx_ptr[i]   = (temp_data2 << 8) | temp_data1;
		if( ab->sfx_ptr[i] > max_height ) max_height = ab->sfx_ptr[i];
		if( ab->sfx_ptr[i] < max_depth ) max_depth = ab->sfx_ptr[i];
	}

	sd_fclose(sfx);

	if(VERBOSE)printf("Max height: %i, Max depth: %i\n",max_height,max_depth);

	ab->sfx_size = size;
}

//loads sfx
void preload_sfx1(char* name, audio_buffer *ab) {

	sdf sfx = init_read_wav(name);

	short int temp_data1, temp_data2;
	int i;
	int size = 0;
	short int max_height = 0;
	short int max_depth = 0;

	//checks length
	while(1)
	{
		temp_data1 = sd_readbyte(sfx);
		if( temp_data1 < 0 ) { //reached EOF
			break;
		}
		sd_readbyte(sfx);
		size++;
	}

	sd_fclose(sfx);

	ab->sfx_ptr1 = (short int*) malloc(size*sizeof(short int));

	sfx = init_read_wav(name);
	if(VERBOSE)printf("Size: %i word(2bytes)\n",size);

	for(i = 0; i < size; i++) {
		temp_data1 = sd_readbyte(sfx);
		temp_data2 = sd_readbyte(sfx);

		ab->sfx_ptr1[i]   = (temp_data2 << 8) | temp_data1;
		if( ab->sfx_ptr1[i] > max_height ) max_height = ab->sfx_ptr1[i];
		if( ab->sfx_ptr1[i] < max_depth ) max_depth = ab->sfx_ptr1[i];
	}

	sd_fclose(sfx);

	if(VERBOSE)printf("Max height: %i, Max depth: %i\n",max_height,max_depth);

	ab->sfx_size1 = size;
}

void free_sfx(audio_buffer* ab){
	free(ab->sfx_ptr);
}

void free_sfx1(audio_buffer* ab){
	free(ab->sfx_ptr1);
}

void sd_read_level(char* filename, level* l ) {

	sdf handle = sd_fopen(filename);

	char read[20];
	char temp;
	int read_index, i, obj_index;
	short int byte;

	obj_index = 0;

	//byte = sd_readbyte(handle);
	//temp = byte;
	//printf("read:%i char:%c\n", byte, temp);

	while(1) {
		byte = sd_readbyte(handle);
		temp = byte;
		if(VERBOSE) printf("read:%i char:%c\n", byte, temp);

		if( byte < 0 )
			break; //EOF
		else if(temp == '#') { //comment line; ignore
			if(VERBOSE) printf("entered comment\n");
			while(1){
				byte = sd_readbyte(handle);
				temp = byte;
				if(VERBOSE) printf("read:%i char:%c\n", byte, temp);
				if( temp == '\n' || byte < 0 ) break;
			}
			if(VERBOSE) printf("exited \n");
		}else if(temp == '\n' || temp == ' ' || temp == 13 /*carriage return*/){
			if(VERBOSE) printf("ENTERED do nothing\n");
			//do nothing
		}else {
			if(VERBOSE) printf("ENTERED READ: \n");

			read_index = 0;
			read[read_index++] = temp;

			for(i = 0; i < 6; i++) {

				while(1){
					byte = sd_readbyte(handle);
					temp = byte;

					read[read_index] = byte;

					if(VERBOSE) printf("read:%i char:%c\n", byte, temp);

					if( temp == ' ' || temp == '\n' || byte < 0 ){
						read[read_index] = '\0';

						if( read_index != 0)
							break;
						read_index--; //eat whitespace
					}
					read_index++;
				}
				if( i == 0 )
					l->wait_time[obj_index] = ((unsigned int)atoi((const char*)read)) * ((unsigned int)5000000);
				else if( i == 1 )
					l->obj_type[obj_index] = atoi((const char*)read);
				else if( i == 2 )
					l->height[obj_index] = atoi((const char*)read);
				else if( i == 3 )
					l->x[obj_index] = atoi((const char*)read);
				else if( i == 4 )
					l->y[obj_index] = atoi((const char*)read);
				else if( i == 5)
					l->color[obj_index] = atoi((const char*)read);

				read_index = 0;
			}
			obj_index++;
			if(VERBOSE) printf("exited \n");
		}
	}
	if(VERBOSE) printf("function exited\n");

	l->obj_index = 0;
	l->size = obj_index;

	int m;
	for(m=0;m<obj_index;m++){
		printf("%i %i %i %i %i\n",l->wait_time[m],l->obj_type[m],l->height[m],l->x[m],l->y[m]);
	}

	printf("size: %i\n",obj_index);

	sd_fclose(handle);
}

void sd_read_players(char* filename, player_data* p, int num_level ) {

	sdf handle = sd_fopen(filename);

	char read[MAX_NAME_LENGTH + 1];

	char temp;
	short int byte;

	int player_index = 0;
	int score_index = 0;
	int read_index = 0;
	int i;

	while(1) {
		byte = sd_readbyte(handle);
		temp = byte;

		if(VERBOSE) printf("read:%i char:%c\n", byte, temp);

		if( byte < 0 || temp == ';' )
			break; //EOF
		else if(temp == '#') { //comment line; ignore
			if(VERBOSE) printf("entered comment\n");
			while(1){
				byte = sd_readbyte(handle);
				temp = byte;
				if(VERBOSE) printf("read:%i char:%c\n", byte, temp);
				if( temp == '\n' || byte < 0 ) break;
			}
			if(VERBOSE) printf("exited \n");
		}else if(temp == '\n' || temp == ' ' || temp == 13 /*carriage return*/){
			if(VERBOSE) printf("ENTERED do nothing\n");
			//do nothing
		}else {
			if(VERBOSE) printf("ENTERED READ: \n");

			read_index = 0;
			read[read_index++] = temp;

			for(i = 0; i < num_level + 1; i++) {

				//this reads data until we hit a space
				while(1){
					byte = sd_readbyte(handle);
					temp = byte;

					read[read_index] = byte;

					if(VERBOSE) printf("read:%i char:%c\n", byte, temp);

					if( temp == ' ' || temp == '\n' || temp == ';' || byte < 0 ){
						read[read_index] = '\0';

						if( read_index != 0)
							break;
						read_index--; //eat whitespace
					}
					read_index++;
				}

				//converts data into the right format
				if( i == 0 ) {
					int j;
					for(j = 0; j < MAX_NAME_LENGTH + 1; j++) {
						p->names[player_index][j] = read[j];
					}
				}else {
					p->scores[player_index][score_index] = atoi((const char*)read);
					score_index++;
				}

				read_index = 0;
			}
			player_index++;
			score_index = 0;
			if(VERBOSE) printf("exited \n");
		}
	}
	if(VERBOSE) printf("function exited\n");

	p->num_level = num_level;
	p->num_player = player_index;

	sd_fclose(handle);
}

void sd_write_players(char* filename, player_data* p ) {

	sdf handle = sd_fopen(filename);
	char temp[30];
	if(VERBOSE) printf("write entered\n");

	//flushes data
	//printf("DATDATA: %i\n",sd_readbyte(handle));
	//while(sd_readbyte(handle) != -1);


	int i,j,k,l;
	bool c;

	for(i=0; i < p->num_player; i++){
		for(j = 0; p->names[i][j] != '\0'; j++) {
			c = alt_up_sd_card_write(handle, p->names[i][j]);
			if(c && VERBOSE) printf("write success\n");
			else if(VERBOSE) printf("write no\n");
		}
		alt_up_sd_card_write(handle, ' ');

		for(k = 0; k < p->num_level; k++) {
			sprintf(temp, "%i", p->scores[i][k]);

			for(l = 0; temp[l] != '\0'; l++) {
				alt_up_sd_card_write(handle,temp[l]);
			}
			alt_up_sd_card_write(handle, ' ');
		}
		alt_up_sd_card_write(handle, 13);
		alt_up_sd_card_write(handle, '\n');
	}

	alt_up_sd_card_write(handle, ';');
	while(sd_readbyte(handle) != -1); //seems to be some bug with the sd card
	//lazy fix
	alt_up_sd_card_write(handle, ';');

	sd_fclose(handle);
	if(VERBOSE) printf("write exited\n");
}

bmp* read_bmp(char* name) {

	//allocate the struct
	bmp* b = (bmp*)malloc(sizeof(bmp));

	//length of BMP header
	const int HEADER_END_ADDR = 0x1A;

	//allocate header to be read in
	unsigned char bmp_header[HEADER_END_ADDR];
	int i, j, k, offset;


	//open file
	sdf h = sd_fopen(name);

redo:
	//read in header indo
	for(i = 0; i < HEADER_END_ADDR; i++) {
		bmp_header[i] = (unsigned char)sd_readbyte(h);
		if(V_VERBOSE) printf("%.2X ", bmp_header[i]);
	}
	if(V_VERBOSE) printf("\n");

	//PARSE HEADER
	//get the rest of offset
	offset = (int)*(unsigned short int*)(bmp_header+0xA);
	//get width (x)
	b->x = (int)*(unsigned short int*)(bmp_header+0x12);
	//get height (y)
	b->y = (int)*(unsigned short int*)(bmp_header+0x16);

	if( b->x > 2000 || b->y > 240 || b->x < 0 || b->y < 0 ){
		sd_fclose(h);
		h = sd_fopen(name);
		goto redo;
		printf("BMP ERROR!\n");
	}

	//calculate amount of data to discard after each row
	//as each row must be a multiple of 4 bytes
	k = (4 - ((3 * b->x) % 4)) % 4;

	if(V_VERBOSE) printf("offset: %X(in hex) width: %i height: %i pad: %i\n", offset,b->x,b->y,k);

	//allocate appropriate memory
	b->pic = (short int**)malloc(b->y*sizeof(short int*)); //y# of pointers
	for(i = 0; i < b->y; i++) {
		b->pic[i] = (short int*)malloc(b->x*sizeof(short int));
	}

	//discard the rest of the offset data
	for(i = HEADER_END_ADDR; i < offset; i++)
		sd_readbyte(h);

	//temp data
	char red, green, blue;

	//reading bmp data
	for(i = b->y - 1; i >= 0; i--) { //reverse the image to the correct orientation
		for(j = 0; j < b->x; j++) {

			blue = sd_readbyte(h); //blue
			green = sd_readbyte(h); //green
			red = sd_readbyte(h); //red

			b->pic[i][j] = ((short int)(red&0xf8)<<8)|((short int)(green&0xfc)<<3)|((short int)(blue&0xf8)>>3);
		}
		//discard additional (padding) data
		for(j = 0; j < k; j++) {
			sd_readbyte(h);
		}
	}

	//closes the file handle
	sd_fclose(h);

	return b;
}

//free bitmap memory
//untested
void free_bmp(bmp *b) {
	int i;
	for(i = 0; i < b->y; i++) {
		free(b->pic[i]);
	}
	free(b->pic);
	free(b);
}

//x & y are coordinate at the top left corner
//pure white (eg. 0xFFFF) are counted as invisible pixel
void draw_bmp(alt_up_pixel_buffer_dma_dev *pixel_buffer, int x, int y, bmp* b) {
	unsigned int i,j,k,l;
	unsigned int addr = pixel_buffer->back_buffer_start_address;
	unsigned int limit_x = pixel_buffer->x_resolution;
	short int** pic = b->pic;
	short int temp;

	//compute starting address in y
	limit_x = limit_x << 1;
	addr += y * (limit_x);

	//if(V_VERBOSE) printf("start bmp draw\n");

	for(i = y, k = 0; i < y + b->y; i++, k++) {
		for (j = x, l = 0; j < x + b->x; j++, l++)
		{
			//alternative pixel drawing function
			//alt_up_pixel_buffer_dma_draw(pixel_buffer, pic[k][l], l, k);

			//draw pixel
			temp = pic[k][l];
			if(pic[k][l] != 0x789) //if pure white, don't draw
				IOWR_16DIRECT(addr, j << 1, temp);
		}
		addr += limit_x;

		//if(V_VERBOSE) printf("finished line %i\n", k);
	}
	//if(V_VERBOSE) printf("ended bmp draw\n");
}


void clear_loc(alt_up_pixel_buffer_dma_dev *pixel_buffer, int x0, int y0, int x1, int y1, bmp* b) {
	unsigned int i,j,k;
	unsigned int addr = pixel_buffer->back_buffer_start_address;
	unsigned int limit_x = pixel_buffer->x_resolution;


	//compute starting address in y
	limit_x = limit_x << 1;
	addr += y0 * (limit_x);

	for(i = y0; i <= y1 ; i++) {
		for (j = x0, k = x0 + p_shift*2; j <= x1 ; j++, k++)
		{

			IOWR_16DIRECT(addr, j << 1, b->pic[i][k]);
		}
		addr += limit_x;
	}
}

/*
void unrolled_parallax_draw(alt_up_pixel_buffer_dma_dev *pixel_buffer, bmp* b) {
	register unsigned int i,j,a,k;
	const unsigned int addr = pixel_buffer->back_buffer_start_address;
	const unsigned int limit_x = pixel_buffer->x_resolution << 1;

	register int* fast;
	a = addr;

	if(p_shift*2 +320 == b->x) p_shift = 0;
	//printf("pshift: %i\n",p_shift);

	for(i = 0; i <= 239 ; i++) {

		fast = (int*)(b->pic[i]);
		for (j = 0, k = p_shift; j < 160 ; j+=10, k+=10)
		{
			IOWR_32DIRECT(a,     j << 2, fast[k]);
			IOWR_32DIRECT(a, (j+1) << 2, fast[k+1]);
			IOWR_32DIRECT(a, (j+2) << 2, fast[k+2]);
			IOWR_32DIRECT(a, (j+3) << 2, fast[k+3]);
			IOWR_32DIRECT(a, (j+4) << 2, fast[k+4]);

			IOWR_32DIRECT(a, (j+5) << 2, fast[k+5]);
			IOWR_32DIRECT(a, (j+6) << 2, fast[k+6]);
			IOWR_32DIRECT(a, (j+7) << 2, fast[k+7]);
			IOWR_32DIRECT(a, (j+8) << 2, fast[k+8]);
			IOWR_32DIRECT(a, (j+9) << 2, fast[k+9]);

//			IOWR_32DIRECT(a, (j+10) << 2, fast[k+10]);
//			IOWR_32DIRECT(a, (j+11) << 2, fast[k+11]);
//			IOWR_32DIRECT(a, (j+12) << 2, fast[k+12]);
//			IOWR_32DIRECT(a, (j+13) << 2, fast[k+13]);
//			IOWR_32DIRECT(a, (j+14) << 2, fast[k+14]);
//
//			IOWR_32DIRECT(a, (j+15) << 2, fast[k+15]);
//			IOWR_32DIRECT(a, (j+16) << 2, fast[k+16]);
//			IOWR_32DIRECT(a, (j+17) << 2, fast[k+17]);
//			IOWR_32DIRECT(a, (j+18) << 2, fast[k+18]);
//			IOWR_32DIRECT(a, (j+19) << 2, fast[k+19]);
		}
		a += limit_x;
	}
}*/
