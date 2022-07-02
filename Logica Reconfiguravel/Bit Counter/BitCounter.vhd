library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity BitCounter is
  port (
    clock    	: in std_logic;
    reset    	: in std_logic;
    entrada     : in std_logic_vector (3 downto 0);
    sinal_for	: out std_logic_vector (2 downto 0);
	sinal_while	: out std_logic_vector (2 downto 0);
    variavel  	: out std_logic_vector (2 downto 0)
  );
end entity;

architecture behavioral of BitCounter is
begin
    process(reset, clock)
        variable j, k, x: integer;
    begin
        if reset = '1' then
            j := 0;
			x := 0;
            sinal_for <= "000";
			sinal_while <= "000";
            variavel <= "000";
        elsif clock'event and clock = '1' then
            j := 0;
            for i in 0 to entrada'length - 1 loop
                if entrada(i) = '1' then
                    j := j + 1;
                end if;
            end loop;
            sinal_for <= std_logic_vector(to_unsigned(j, sinal_for'length));

			x := 0;
			k := 0;
			while k <= sinal_while'length loop
				if entrada(k) = '1' then
                    x := x + 1;
                end if;
				k := k + 1;
			end loop;
			sinal_while <= std_logic_vector(to_unsigned(x, sinal_while'length));
			
            case entrada is
                when "0000" => variavel <= "000";
                when "0001" => variavel <= "001";
                when "0010" => variavel <= "001";
                when "0011" => variavel <= "010";
                when "0100" => variavel <= "001";
                when "0101" => variavel <= "010";
                when "0110" => variavel <= "010";
                when "0111" => variavel <= "011";
                when "1000" => variavel <= "001";
                when "1001" => variavel <= "010";
                when "1010" => variavel <= "010";
                when "1011" => variavel <= "011";
                when "1100" => variavel <= "010";
                when "1101" => variavel <= "011";
                when "1110" => variavel <= "011";
                when "1111" => variavel <= "100";
                when others => variavel <= "000";
            end case;
        end if;
    end process;
end architecture;