library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Timer is
    generic(period : integer := 1);     -- Timer period in milliseconds.

    port(
        clock  : in  std_logic;
        reset  : in  std_logic;
        enable : in  std_logic;
        tick   : out std_logic
    );
end entity;

architecture behavioral of Timer is
    constant frequency : integer := 50000;
    signal i : integer;
begin
    process(reset, clock)
    begin
        if reset = '0' then
            i <= 1;
            tick <= '0';
        elsif rising_edge(clock) then
            if enable = '1' then
                if i = period * frequency then
                    i <= 1;
                    tick <= '1';
                else
                    i <= i + 1;
                    tick <= '0';
                end if;
            end if;
        end if;
    end process;
end architecture;
