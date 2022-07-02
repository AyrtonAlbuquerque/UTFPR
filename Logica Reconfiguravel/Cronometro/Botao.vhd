library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity Botao is
    generic (periodo : integer := 30);  -- Periodo de debounce em ms. 30 ms Padrão.

    port(
        clock   : in std_logic;
        reset   : in std_logic;
		hold	: in std_logic;
        entrada : in std_logic;
        saida   : out std_logic
    );
end entity;

architecture behavioral of Botao is
	signal estado: std_logic;
begin
    process(reset, clock)
		variable i: integer := 0;
    begin
        if reset = '1' then
			estado <= '0';
			i := 0;
        elsif clock'event and clock = '1' then
			if entrada = '0' then
				if i < periodo then
					i := i + 1;
					if i = periodo then
						if hold = '0' then
							estado <= '1';
						else
							estado <= not estado;
						end if;
					end if;
				end if;
			else
				i := 0;
				if hold = '0' then
					estado <= '0';
				end if;
			end if;
        end if;
    end process;
	
	saida <= estado;
end architecture;