LIBRARY ieee;
USE ieee.std_logic_1164.all; 
USE ieee.std_logic_arith.all; 
USE ieee.std_logic_unsigned.all; 

entity module_two is
	port (
		SW : IN std_logic_vector(0 downto 0);
      CLOCK_50 : IN std_logic;
      DRAM_CLK, DRAM_CKE : OUT std_logic;
      DRAM_ADDR : OUT std_logic_vector(11 DOWNTO 0);
      DRAM_BA_0, DRAM_BA_1 : BUFFER std_logic;
      DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT std_logic;
      DRAM_DQ : INOUT std_logic_vector(15 DOWNTO 0);
      DRAM_UDQM, DRAM_LDQM : BUFFER std_logic;
		
		SD_CMD : inout std_logic;
		SD_DAT : inout std_logic;
		SD_DAT3 : inout std_logic;
		SD_CLK : out std_logic;
		
		UART_RXD : in std_logic;
		UART_TXD : out std_logic
	);
end module_two;

architecture module_two_structure of module_two is
	component nios_system_LCD_SDCARD_Buttons_SCREEN port (
			clk_clk : in std_logic;
			reset_reset_n : in std_logic;
			
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
			
			sd_card_interface_b_SD_cmd : inout std_logic;
			sd_card_interface_b_SD_dat : inout std_logic;
			sd_card_interface_b_SD_dat3 : inout std_logic;
			sd_card_interface_o_SD_clock : out std_logic;
			
			rs232_external_interface_RXD : in std_logic;
			rs232_external_interface_TXD : out std_logic);
			
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
		 
		 sd_card_interface_b_SD_cmd => SD_CMD,
		 sd_card_interface_b_SD_dat => SD_DAT,
		 sd_card_interface_b_SD_dat3 => SD_DAT3,
		 sd_card_interface_o_SD_clock => SD_CLK,

		 rs232_external_interface_RXD => UART_RXD,
		 rs232_external_interface_TXD => UART_TXD);
		 
end module_two_structure;
	

	
	
	
	
	
	