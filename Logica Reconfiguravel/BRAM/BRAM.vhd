library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity BRAM is
    port(
        reset       : in std_logic;
        clock       : in std_logic;
        wren        : in std_logic;
        rden        : in std_logic;
        chipselect  : in std_logic;
        address     : in std_logic_vector (9 downto 0);
        write       : in std_logic_vector (7 downto 0);
        read        : out std_logic_vector (7 downto 0)
    );
end entity;

architecture behavioral of BRAM is
    signal output : std_logic_vector (7 downto 0);
    
    component RAM is
        port
        (
            address : IN std_logic_vector (9 DOWNTO 0);
            clock   : IN std_logic ;
            data    : IN std_logic_vector (7 DOWNTO 0);
            wren    : IN std_logic ;
            q       : OUT std_logic_vector (7 DOWNTO 0)
        );
    end component;
begin
    memory: RAM port map(address, clock, write, wren, output);
    
    process(reset, clock, chipselect)
    begin
        if reset = '1' then
            read <= "00000000";
        elsif chipselect = '1' then
            if clock'event and clock = '1' then
                if rden = '1' then
                    read <= output;
                else
                    read <= "00000000";
                end if;
            end if;
        end if;
    end process;
end architecture;