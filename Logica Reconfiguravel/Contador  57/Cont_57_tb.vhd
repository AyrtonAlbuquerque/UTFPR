library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all; 

entity Cont_57_tb is
end entity;

architecture behavioral of Cont_57_tb is
    component Cont_57 is
        port
        (
            clock  : IN std_logic ;
            clear  : IN std_logic ;
            reset  : IN std_logic ;
            enable : IN std_logic ;
            qu     : OUT std_logic_vector (3 downto 0);
            qd     : OUT std_logic_vector (3 downto 0)
        );
    end component;
    
    signal clock, clear, reset, enable: std_logic;
    signal qu, qd: std_logic_vector (3 downto 0);
begin
    uut: Cont_57 port map(clock, clear, reset, enable, qu, qd);
    
    enable <= '1';
    clear <= '0';
    
    process
    begin
        reset <= '1';
        wait for 15 ns;
        reset <= '0';
        wait;
    end process;
    
    process
    begin
        clock <= '1';
        wait for 20 ns;
        clock <= '0';
        wait for 20 ns;
    end process;
end architecture; 