library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity Cont_57 is
    port(
        clock   : in std_logic;
        clear   : in std_logic;
        reset   : in std_logic;
        enable  : in std_logic;
        qu      : out std_logic_vector(3 downto 0);
        qd      : out std_logic_vector(3 downto 0)
    );
end entity;

architecture behavioral of Cont_57 is
    component Contador is
        port
        (
            clock  : in std_logic ;
            clear  : in std_logic ;
            reset  : in std_logic ;
            set    : in std_logic ;
            enable : in std_logic ;
            load   : in std_logic_vector (3 downto 0);
            q      : out std_logic_vector (3 downto 0)
        );
    end component;
    
    signal uSet, dSet, dClock, uReset: std_logic;
    signal uLoad, dLoad, q0, q1: std_logic_vector (3 downto 0);
begin
    unidade: Contador port map(clock, clear, uReset, uSet, enable, uLoad, q0);
    dezena: Contador port map(dClock, clear, reset, dSet, enable, dLoad, q1);
    
    uSet <= '1' when (q1 = "0110" and q0 = "1000") else '0';
    dSet <= '1' when (q1 = "0000" or q1 = "0110") else '0';
    
    uLoad <= "0010" when uSet = '1' else "0000";
    dLoad <= "0001";
    
    dClock <= '1' when (q0 = "0000" or (q0 = "0010" and uSet = '1')) else '0';
    uReset <= '1' when (q0 = "1010" or reset = '1') else '0';
    
    qu <= q0;
    qd <= q1;
end architecture;