library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity Frequencia is
    generic (periodo : integer := 1);    -- Period desejado em ms. 1 ms padrão.
    
    port(
        sinal : in std_logic;
        reset : in std_logic;
        clock : out std_logic
    );
end entity;

architecture behavioral of Frequencia is
    constant base : integer := 50000;
    signal i : integer;
begin
    process(reset, sinal)
    begin
        if reset = '1' then
            i <= 1;
            clock <= '0';
        elsif sinal'event and sinal = '1' then
            if periodo > 0 then
                if i = (periodo * base) then
                    clock <= '1';
                    i <= 1;
                else
                    i <= i + 1;
                    clock <= '0';
                end if;
            end if;
        end if;
    end process;
end architecture;