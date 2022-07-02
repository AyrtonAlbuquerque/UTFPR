library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all; 

entity Cronometro_tb is
end entity Cronometro_tb;

architecture behavioral of Cronometro_tb is
    signal clock, reset, enable, clear : std_logic;
    signal uc, dc, us, ds : std_logic_vector (6 downto 0);
    
    component Cronometro is
        port
        (
            clock  	: in std_logic;
			reset  	: in std_logic;
			enable 	: in std_logic;
			clear   : in std_logic;
			uc     	: out std_logic_vector (6 downto 0);
			dc     	: out std_logic_vector (6 downto 0);
			us     	: out std_logic_vector (6 downto 0);
			ds     	: out std_logic_vector (6 downto 0)
        );
    end component;
begin
    uut: Cronometro port map(clock, reset, enable, clear, uc, dc, us, ds);
    
	process
	begin
		reset <= '1';
		clear <= '0';
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
		enable <= '0';
		wait for 15 ns;
		enable <= '1';
		wait for 10000 ns;
		enable <= '0';
		wait for 10000 ns;
		enable <= '1';
		wait;
	end process;
end architecture; 