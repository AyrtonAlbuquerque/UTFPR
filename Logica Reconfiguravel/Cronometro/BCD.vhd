library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity BCD is
    port(
        entrada : in std_logic_vector (3 downto 0);
        saida   : out std_logic_vector (6 downto 0)
    );
end entity;

architecture behavioral of BCD is
begin
    process(entrada)
    begin
        case entrada is
            when "0000" => 
                saida <= "0000001";
            when "0001" =>
                saida <= "1001111";
            when "0010" =>
                saida <= "0010010";
            when "0011" =>
                saida <= "0000110";
            when "0100" =>
                saida <= "1001100";
            when "0101" =>
                saida <= "0100100";
            when "0110" =>
                saida <= "0100000";
            when "0111" =>
                saida <= "0001111";
            when "1000" =>
                saida <= "0000000";
            when "1001" =>
                saida <= "0000100";
            when "1010" =>
                saida <= "0001000";
            when "1011" =>
                saida <= "1100000";
            when "1100" =>
                saida <= "0110001";
            when "1101" =>
                saida <= "1000010";
            when "1110" =>
                saida <= "0110000";
            when "1111" =>
                saida <= "0111000";
            when others =>
                saida <= "1111111";
        end case;
    end process;    
end architecture;