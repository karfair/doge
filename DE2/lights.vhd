-- Implements a simple Nios II system for the DE2 board.
-- Inputs: SW7-0 are parallel port inputs to the Nios II system.
-- CLOCK_50 is the system clock.
-- KEY0 is the active-low system reset.
-- Outputs: LEDG7-0 are parallel port outputs from the Nios II system.
-- SDRAM ports correspond to the signals in Figure 2; their names are those
-- used in the DE2 User Manual.

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
USE ieee.std_logic_unsigned.all;

ENTITY lights IS
	PORT (
		SW : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		KEY : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
		CLOCK_50 : IN STD_LOGIC;
		LEDG : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		DRAM_CLK, DRAM_CKE : OUT STD_LOGIC;
		DRAM_ADDR : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
		DRAM_BA_0, DRAM_BA_1 : BUFFER STD_LOGIC;
		DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT STD_LOGIC;
		DRAM_DQ : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		DRAM_UDQM, DRAM_LDQM : BUFFER STD_LOGIC;
		
		--added in tutorial_1.6
		LCD_DATA : inout STD_LOGIC_VECTOR(7 downto 0);
		LCD_ON : out std_logic;
		LCD_BLON : out std_logic;
		LCD_EN : out std_logic;
		LCD_RS : out std_logic;
		LCD_RW : out std_logic;
		--end tutorial_1.6

		--added in sd_card
		SD_CMD : inout std_logic;
		SD_DAT : inout std_logic;
		SD_DAT3 : inout std_logic;
		SD_CLK : out std_logic;
		--end sd_card
		
		--added in VGA_part_a
		SRAM_DQ              : inout std_logic_vector(15 downto 0); -- DQ
		SRAM_ADDR            : out   std_logic_vector(17 downto 0);                    -- ADDR
		SRAM_LB_N            : out   std_logic;                                        -- LB_N
		SRAM_UB_N            : out   std_logic;                                        -- UB_N
		SRAM_CE_N            : out   std_logic;                                        -- CE_N
		SRAM_OE_N            : out   std_logic;                                        -- OE_N
		SRAM_WE_N            : out   std_logic;                                        -- WE_N
		VGA_CLK   : out   std_logic;                                        -- CLK
		VGA_HS    : out   std_logic;                                        -- HS
		VGA_VS    : out   std_logic;                                        -- VS
		VGA_BLANK : out   std_logic;                                        -- BLANK
		VGA_SYNC  : out   std_logic;                                        -- SYNC
		VGA_R     : out   std_logic_vector(9 downto 0);                     -- R
		VGA_G     : out   std_logic_vector(9 downto 0);                     -- G
		VGA_B     : out   std_logic_vector(9 downto 0);                      -- B
	   --end VGA_part_a
		
		--added in audio
		CLOCK_27                     : in    std_logic                     ;             -- clk
		AUD_XCK                  : out   std_logic;                                        -- clk
		AUD_ADCDAT  : in    std_logic                     ;             -- ADCDAT
		AUD_ADCLRCK : in    std_logic                     ;             -- ADCLRCK
		AUD_BCLK    : in    std_logic                     ;             -- BCLK
		AUD_DACDAT  : out   std_logic;                                        -- DACDAT
		AUD_DACLRCK : in    std_logic                     ;             -- DACLRCK
		I2C_SDAT      : inout std_logic                     ;             -- SDAT
		I2C_SCLK      : out   std_logic     ;                                    -- SCLK
		--end audio

		TD_RESET : out std_logic;
		
		--ps2
		ps2_CLK              : inout std_logic                   ;             -- CLK
      ps2_DAT              : inout std_logic  ;                           -- DAT
		
		--rs232
		UART_RXD : in std_logic;
		UART_TXD : out std_logic
		

		
	 );
END lights;



ARCHITECTURE Structure OF lights IS
	COMPONENT nios_system
		PORT (
			clk_clk : IN STD_LOGIC;
			reset_reset_n : IN STD_LOGIC;
			sdram_clk_clk : OUT STD_LOGIC;
			leds_export : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
			switches_export : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
			sdram_wire_addr : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
			sdram_wire_ba : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
			sdram_wire_cas_n : OUT STD_LOGIC;
			sdram_wire_cke : OUT STD_LOGIC;
			sdram_wire_cs_n : OUT STD_LOGIC;
			sdram_wire_dq : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
			sdram_wire_dqm : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
			sdram_wire_ras_n : OUT STD_LOGIC;
			sdram_wire_we_n : OUT STD_LOGIC; 
			
			--added in tutorial 1.6
			lcd_data_DATA : inout STD_LOGIC_VECTOR(7 downto 0);
			lcd_data_on : out std_logic;
			lcd_data_blon : out std_logic;
			lcd_data_en : out std_logic;
			lcd_data_rs : out std_logic;
			lcd_data_rw : out std_logic;

			--added in sd_card
			sd_data_b_SD_cmd : inout std_logic;
			sd_data_b_SD_dat : inout std_logic;
			sd_data_b_SD_dat3 : inout std_logic;
			sd_data_o_SD_clock : out std_logic;
			--end sd_card
			
			--added in VGA_part_a
			sram_DQ              : inout std_logic_vector(15 downto 0); -- DQ
			sram_ADDR            : out   std_logic_vector(17 downto 0);                    -- ADDR
			sram_LB_N            : out   std_logic;                                        -- LB_N
			sram_UB_N            : out   std_logic;                                        -- UB_N
			sram_CE_N            : out   std_logic;                                        -- CE_N
			sram_OE_N            : out   std_logic;                                        -- OE_N
			sram_WE_N            : out   std_logic;                                        -- WE_N
			vga_controller_CLK   : out   std_logic;                                        -- CLK
			vga_controller_HS    : out   std_logic;                                        -- HS
			vga_controller_VS    : out   std_logic;                                        -- VS
			vga_controller_BLANK : out   std_logic;                                        -- BLANK
			vga_controller_SYNC  : out   std_logic;                                        -- SYNC
			vga_controller_R     : out   std_logic_vector(9 downto 0);                     -- R
			vga_controller_G     : out   std_logic_vector(9 downto 0);                     -- G
			vga_controller_B     : out   std_logic_vector(9 downto 0);                      -- B
			--end VGA_part_a
			
			--added in audio
			clock_27_clk                     : in    std_logic                     ;             -- clk
			clock_audio_clk                  : out   std_logic;                                        -- clk
			aud_ADCDAT  : in    std_logic                    ;             -- ADCDAT
			aud_ADCLRCK : in    std_logic                     ;             -- ADCLRCK
			aud_BCLK    : in    std_logic                  ;             -- BCLK
			aud_DACDAT  : out   std_logic;                                        -- DACDAT
			aud_DACLRCK : in    std_logic                    ;             -- DACLRCK
			av_SDAT      : inout std_logic                  ;             -- SDAT
			av_SCLK      : out   std_logic   ;                                      -- SCLK
			--end audio

			--ps2
			ps2_CLK              : inout std_logic                    ;             -- CLK
         ps2_DAT              : inout std_logic   ;                             -- DAT
			
			--key
			pb_export            : in    std_logic_vector(3 downto 0) ;
			
			--android communication
			rs232_RXD            : in    std_logic                     := 'X';             -- RXD
         rs232_TXD            : out   std_logic                                         -- TXD
		);
	END COMPONENT;
	
	SIGNAL DQM : STD_LOGIC_VECTOR(1 DOWNTO 0);
	SIGNAL BA : STD_LOGIC_VECTOR(1 DOWNTO 0);
	
BEGIN
	TD_RESET <= '1';
	DRAM_BA_0 <= BA(0);
	DRAM_BA_1 <= BA(1);
	DRAM_UDQM <= DQM(1);
	DRAM_LDQM <= DQM(0);
	
	-- Instantiate the Nios II system entity generated by the Qsys tool.
	NiosII: nios_system
		PORT MAP (
			clk_clk => CLOCK_50,
			reset_reset_n => KEY(0),
			sdram_clk_clk => DRAM_CLK,
			leds_export => LEDG,
			switches_export => SW,
			sdram_wire_addr => DRAM_ADDR,
			sdram_wire_ba => BA,
			sdram_wire_cas_n => DRAM_CAS_N,
			sdram_wire_cke => DRAM_CKE,
			sdram_wire_cs_n => DRAM_CS_N,
			sdram_wire_dq => DRAM_DQ,
			sdram_wire_dqm => DQM,
			sdram_wire_ras_n => DRAM_RAS_N,
			sdram_wire_we_n => DRAM_WE_N, 
			--added in tutorial 1.6
			lcd_data_DATA => LCD_DATA,
			lcd_data_on => LCD_on,
			lcd_data_blon => LCD_blon,
			lcd_data_en => LCD_en,
			lcd_data_rs => LCD_rs,
			lcd_data_rw => LCD_rw,

			--added in sd_card
			sd_data_b_SD_cmd => sd_cmd,
			sd_data_b_SD_dat => sd_dat,
			sd_data_b_SD_dat3 => sd_dat3,
			sd_data_o_SD_clock => sd_clk,
			--end sd_card
		   
			--added in VGA_part_a
			sram_DQ              => sram_DQ,              --           sram.DQ
			sram_ADDR            => sram_ADDR,            --               .ADDR
			sram_LB_N            => sram_LB_N,            --               .LB_N
			sram_UB_N            => sram_UB_N,            --               .UB_N
			sram_CE_N            => sram_CE_N,            --               .CE_N
			sram_OE_N            => sram_OE_N,            --               .OE_N
			sram_WE_N            => sram_WE_N,            --               .WE_N
			vga_controller_CLK   => vga_CLK,   -- vga_controller.CLK
			vga_controller_HS    => vga_HS,    --               .HS
			vga_controller_VS    => vga_VS,    --               .VS
			vga_controller_BLANK => vga_BLANK, --               .BLANK
			vga_controller_SYNC  => vga_SYNC,  --               .SYNC
			vga_controller_R     => vga_R,     --               .R
			vga_controller_G     => vga_G,     --               .G
			vga_controller_B     => vga_B,      --               .B
			--end VGA_part_a
			
			--added in audio
			clock_27_clk                     => clock_27,                     --                 clock_27.clk
			clock_audio_clk                  => aud_xck,                  --              clock_audio.clk
			aud_ADCDAT  => aud_ADCDAT,  -- audio_external_interface.ADCDAT
			aud_ADCLRCK => aud_ADCLRCK, --                         .ADCLRCK
			aud_BCLK    => aud_BCLK,    --                         .BCLK
			aud_DACDAT  => aud_DACDAT,  --                         .DACDAT
			aud_DACLRCK => aud_DACLRCK, --                         .DACLRCK
			av_SDAT      => i2c_SDAT,      --   audio_and_video_config.SDAT
			av_SCLK      => i2c_SCLK,       --                         .SCLK
			--end audio
			
			--ps2
			ps2_CLK              => ps2_CLK,              --            ps2.CLK
         ps2_DAT              => ps2_DAT,               --               .DAT
			
			--key
			pb_export            => key,             --             pb.export
			
			--android communication
			rs232_RXD            => UART_RXD,            --          rs232.RXD
         rs232_TXD            => UART_TXD             --               .TXD
		);
END Structure;