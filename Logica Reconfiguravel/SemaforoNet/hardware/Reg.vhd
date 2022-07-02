library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Reg is
    port(
        clock : in  std_logic;
        reset : in  std_logic;
        wren  : in  std_logic;
        d     : in  std_logic_vector(31 downto 0);
        q     : out std_logic_vector(31 downto 0)
    );
end entity Reg;

architecture behavioral of Reg is
begin
    process(reset, clock)
    begin
        if reset = '0' then
            q <= x"00000000";
        elsif rising_edge(clock) then
            if wren = '1' then
                q <= d;
            end if;
        end if;
    end process;
end architecture;
