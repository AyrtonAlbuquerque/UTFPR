library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity Contador is
    port(
        clock  : in std_logic;
        reset  : in std_logic;
        enable : in std_logic;
        q      : out std_logic_vector (3 downto 0)
    );
end entity;

architecture behavioral of Contador is
    signal i: integer;
begin
    process(clock, reset)
    begin
        if reset = '1' then
            i <= 0;
        elsif clock'event and clock = '1' then
            if enable = '1' then
                i <= i + 1;
            end if;
        end if;
    end process;
    
    q <= std_logic_vector(to_signed(i, q'length));
end architecture;