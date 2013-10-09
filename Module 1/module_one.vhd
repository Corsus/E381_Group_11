LIBRARY ieee;
USE ieee.std_logic_1164.all; 
USE ieee.std_logic_arith.all; 
USE ieee.std_logic_unsigned.all; 

entity module_one is
	port (
		SW : IN std_logic_vector(0 downto 0);
      KEY : IN std_logic_vector(3 DOWNTO 0);
      CLOCK_50 : IN std_logic;
      DRAM_CLK, DRAM_CKE : OUT std_logic;
      DRAM_ADDR : OUT std_logic_vector(11 DOWNTO 0);
      DRAM_BA_0, DRAM_BA_1 : BUFFER std_logic;
      DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT std_logic;
      DRAM_DQ : INOUT std_logic_vector(15 DOWNTO 0);
      DRAM_UDQM, DRAM_LDQM : BUFFER std_logic;
	
		LCD_DATA : INOUT std_logic_vector(7 DOWNTO 0);
		LCD_ON : OUT std_logic;
		LCD_BLON : OUT std_logic;
		LCD_EN : OUT std_logic;
		LCD_RS : OUT std_logic;
		LCD_RW : OUT std_logic;
		
		SD_CMD : inout std_logic;
		SD_DAT : inout std_logic;
		SD_DAT3 : inout std_logic;
		SD_CLK : out std_logic;
		
		VGA_R:out std_logic_vector(9 downto 0);
		VGA_G:out std_logic_vector(9 downto 0);
		VGA_B:out std_logic_vector(9 downto 0);
		VGA_CLK:out std_logic;
		VGA_BLANK:out std_logic;
		VGA_HS:out std_logic;
		VGA_VS:out std_logic;
		VGA_SYNC:out std_logic;
		SRAM_DQ:INOUT STD_LOGIC_VECTOR(15 downto 0);
		SRAM_ADDR:OUT STD_LOGIC_VECTOR(17 downto 0);
		SRAM_LB_N:OUT STD_LOGIC;
		SRAM_UB_N:OUT STD_LOGIC;
		SRAM_CE_N:OUT STD_LOGIC;
		SRAM_OE_N:OUT STD_LOGIC;
		SRAM_WE_N:OUT STD_LOGIC;
		
		I2C_SDAT : inout std_logic;
		I2C_SCLK : out std_logic;
		
		AUD_XCK : out std_logic;
		CLOCK_27 : in std_logic;
		
		AUD_ADCDAT : in std_logic;
		AUD_ADCLRCK : in std_logic;
		AUD_BCLK : in std_logic;
		AUD_DACDAT : out std_logic;
		AUD_DACLRCK : in std_logic
	);
end module_one;

architecture module_one_structure of module_one is
	component nios_system_LCD_SDCARD_Buttons_SCREEN port (
			clk_clk : in std_logic;
			reset_reset_n : in std_logic;
			keys_export : in std_logic_vector(3 downto 0);
			
			sdram_clk_clk : out std_logic;
			sdram_wire_addr : out std_logic_vector(11 downto 0);
			sdram_wire_ba : buffer std_logic_vector(1 downto 0);
			sdram_wire_cas_n : out std_logic;
			sdram_wire_cke : out std_logic;
			sdram_wire_cs_n : out std_logic;
			sdram_wire_dq : inout std_logic_vector(15 downto 0);
			sdram_wire_dqm : buffer std_logic_vector(1 downto 0);
			sdram_wire_ras_n : out std_logic;
			sdram_wire_we_n : out std_logic;
			
			lcd_data_DATA : inout std_logic_vector(7 downto 0);
			lcd_data_ON : out std_logic;
			lcd_data_BLON : out std_logic;
			lcd_data_EN : out std_logic;
			lcd_data_RS : out std_logic;
			lcd_data_RW : out std_logic;	
			
			sd_card_interface_b_SD_cmd : inout std_logic;
			sd_card_interface_b_SD_dat : inout std_logic;
			sd_card_interface_b_SD_dat3 : inout std_logic;
			sd_card_interface_o_SD_clock : out std_logic;
			
			vga_controller_interface_CLK : out STD_LOGIC;
			vga_controller_interface_HS : out STD_LOGIC; 
			vga_controller_interface_VS : out STD_LOGIC; 
			vga_controller_interface_BLANK : out STD_LOGIC;
			vga_controller_interface_SYNC : out STD_LOGIC; 
			vga_controller_interface_R : out STD_LOGIC_VECTOR(9 downto 0);
			vga_controller_interface_G : out STD_LOGIC_VECTOR(9 downto 0); 
			vga_controller_interface_B : out STD_LOGIC_VECTOR(9 downto 0);
			
			pixel_buffer_interface_DQ : inout STD_LOGIC_VECTOR(15 downto 0); 
			pixel_buffer_interface_ADDR : out STD_LOGIC_VECTOR(17 downto 0);
			pixel_buffer_interface_LB_N : out STD_LOGIC; 
			pixel_buffer_interface_UB_N : out STD_LOGIC;
			pixel_buffer_interface_CE_N : out STD_LOGIC;
			pixel_buffer_interface_OE_N : out STD_LOGIC; 
			pixel_buffer_interface_WE_N : out STD_LOGIC;
			
			av_config_interface_SDAT : inout STD_LOGIC;
			av_config_interface_SCLK : out STD_LOGIC;
			
			audio_interface_ADCDAT : in STD_LOGIC;
			audio_interface_ADCLRCK : in STD_LOGIC;
			audio_interface_BCLK : in STD_LOGIC;
			audio_interface_DACDAT : out STD_LOGIC;
			audio_interface_DACLRCK : in STD_LOGIC;
			
			audio_out_clk_clk : out STD_LOGIC;
			audio_in_clk_clk : in STD_LOGIC);
			
	end component;
	
   signal DQM : std_logic_vector(1 DOWNTO 0);
   signal BA : std_logic_vector(1 DOWNTO 0);
	
begin
	DRAM_BA_0 <= BA(0);
	DRAM_BA_1 <= BA(1);
	DRAM_UDQM <= DQM(1);
	DRAM_LDQM <= DQM(0);
	
	NiosII : nios_system_LCD_SDCARD_Buttons_SCREEN port map (
	
		 clk_clk => CLOCK_50,
		 reset_reset_n => not(SW(0)),   --change
		 keys_export => KEY(3 downto 0),
		 
		 sdram_clk_clk => DRAM_CLK,
		 sdram_wire_addr => DRAM_ADDR,
		 sdram_wire_ba => BA,
		 sdram_wire_cas_n => DRAM_CAS_N,
		 sdram_wire_cke => DRAM_CKE,
		 sdram_wire_cs_n => DRAM_CS_N,
		 sdram_wire_dq => DRAM_DQ,
		 sdram_wire_dqm => DQM,
		 sdram_wire_ras_n => DRAM_RAS_N,
		 sdram_wire_we_n => DRAM_WE_N,
		 
		 lcd_data_DATA => LCD_DATA,
		 lcd_data_ON => LCD_ON,
		 lcd_data_BLON => LCD_BLON,
		 lcd_data_EN => LCD_EN,
		 lcd_data_RS => LCD_RS,
		 lcd_data_RW => LCD_RW,
		 
		 sd_card_interface_b_SD_cmd => SD_CMD,
		 sd_card_interface_b_SD_dat => SD_DAT,
		 sd_card_interface_b_SD_dat3 => SD_DAT3,
		 sd_card_interface_o_SD_clock => SD_CLK,
		 
		 vga_controller_interface_CLK => VGA_CLK,
		 vga_controller_interface_HS => VGA_HS,
		 vga_controller_interface_VS => VGA_VS,
		 vga_controller_interface_BLANK => VGA_BLANK,
		 vga_controller_interface_SYNC => VGA_SYNC,
		 vga_controller_interface_R => VGA_R,
		 vga_controller_interface_G => VGA_G,
		 vga_controller_interface_B => VGA_B,
		 
		 pixel_buffer_interface_DQ => SRAM_DQ,
		 pixel_buffer_interface_ADDR => SRAM_ADDR,
		 pixel_buffer_interface_LB_N => SRAM_LB_N,
		 pixel_buffer_interface_UB_N => SRAM_UB_N,
		 pixel_buffer_interface_CE_N => SRAM_CE_N,
		 pixel_buffer_interface_OE_N => SRAM_OE_N,
		 pixel_buffer_interface_WE_N => SRAM_WE_N,
		 
		 av_config_interface_SDAT => I2C_SDAT,
		 av_config_interface_SCLK => I2C_SCLK,
			
		 audio_interface_ADCDAT => AUD_ADCDAT,
		 audio_interface_ADCLRCK => AUD_ADCLRCK,
		 audio_interface_BCLK => AUD_BCLK,
		 audio_interface_DACDAT => AUD_DACDAT,
		 audio_interface_DACLRCK => AUD_DACLRCK,
			
		 audio_out_clk_clk => AUD_XCK,
		 audio_in_clk_clk => CLOCK_27);
		 
end module_one_structure;
	

	
	
	
	
	
	