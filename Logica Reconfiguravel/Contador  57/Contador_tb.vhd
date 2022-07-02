library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity Contador_tb is
end entity Contador_tb;

architecture behavioral of Contador_tb is
    component Contador is
        port
        (
            clock  : IN std_logic ;
            clear  : IN std_logic ;
            reset  : IN std_logic ;
            set    : IN std_logic ;
            enable : IN std_logic ;
            load   : IN std_logic_vector (3 downto 0);
            q      : OUT std_logic_vector (3 downto 0)
        );
    end component;
    
    signal rst, clk, clr, en, ld : std_logic;
    signal load, q : std_logic_vector (3 downto 0);
begin
    uut: Contador port map(clk, clr, rst, ld, en, load, q);
    
    process
    begin
        rst <= '1';
        wait for 15 ns;
        rst <= '0';
        wait;
    end process;
    
    process
    begin
        clk <= '0';
        wait for 20 ns;
        clk <= '1';
        wait for 10 ns;
    end process;
    
    en <= '1';
    clr <= '0';
    ld <= '0';
    load <= "0000";
end architecture;