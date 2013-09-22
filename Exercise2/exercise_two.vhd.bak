LIBRARY ieee;
USE ieee.std_logic_1164.all; 
USE ieee.std_logic_arith.all; 
USE ieee.std_logic_unsigned.all; 

entity exercise_one is
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
		LCD_RW : OUT std_logic
	 
	);
end exercise_one;

architecture exercise_one_structure of exercise_one is
	component nio_system_SDRAM_LCD port (
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
			lcd_data_RW : out std_logic);	
	end component;

   signal DQM : std_logic_vector(1 DOWNTO 0);
   signal BA : std_logic_vector(1 DOWNTO 0);
	
begin
	DRAM_BA_0 <= BA(0);
	DRAM_BA_1 <= BA(1);
	DRAM_UDQM <= DQM(1);
	DRAM_LDQM <= DQM(0);
	
	NiosII : nio_system_SDRAM_LCD port map (
	
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
		 lcd_data_RW => LCD_RW);
end exercise_one_structure;
	

	
	
	
	
	
	