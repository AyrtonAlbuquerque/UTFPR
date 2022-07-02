library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity BitCounter_tb is
end entity;

architecture behavioral of BitCounter_tb is
    signal clock, reset : std_logic;
	signal variavel, sinal_for, sinal_while : std_logic_vector (2 downto 0);
    signal entrada : std_logic_vector (3 downto 0);
    
    component BitCounter is
		port (
			clock    	: in std_logic;
			reset    	: in std_logic;
			entrada     : in std_logic_vector (3 downto 0);
			sinal_for	: out std_logic_vector (2 downto 0);
			sinal_while	: out std_logic_vector (2 downto 0);
			variavel  	: out std_logic_vector (2 downto 0)
		);
    end component;
begin
    uut: BitCounter port map(clock, reset, entrada, sinal_for, sinal_while, variavel);

    process
	begin
		reset <= '1';
		wait for 15 ns;
		reset <= '0';
		wait;
	end process;
	
    process
    begin
        clock <= '0';
        wait for 10 ns;
        clock <= '1';
        wait for 10 ns;
    end process;

    process
    begin
        entrada <= "0000";
        wait for 25 ns;
        entrada <= "0001";
        wait for 25 ns;
        entrada <= "0010";
        wait for 25 ns;
        entrada <= "0011";
        wait for 25 ns;
        entrada <= "0100";
        wait for 25 ns;
        entrada <= "0101";
        wait for 25 ns;
        entrada <= "0110";
        wait for 25 ns;
        entrada <= "0111";
        wait for 25 ns;
        entrada <= "1000";
        wait for 25 ns;
        entrada <= "1001";
        wait for 25 ns;
        entrada <= "1010";
        wait for 25 ns;
        entrada <= "1011";
        wait for 25 ns;
        entrada <= "1100";
        wait for 25 ns;
        entrada <= "1101";
        wait for 25 ns;
        entrada <= "1110";
        wait for 25 ns;
        entrada <= "1111";
        wait;
    end process;
end architecture;