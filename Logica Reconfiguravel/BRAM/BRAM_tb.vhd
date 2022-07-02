library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity BRAM_tb is
end entity;

architecture behavioral of BRAM_tb is
    type states is (idle0, idle1, write0, write1, read0, read1);
    type ayrton is array(27 downto 0) of std_logic_vector(7 downto 0);
    
    signal reset, clock, wren, rden, chipselect : std_logic;
    signal write, read : std_logic_vector (7 downto 0);
    signal address : std_logic_vector (9 downto 0);
    signal counter, index : integer := 0;
	signal i : integer := 0;
    signal state : states;
    signal nome : ayrton := (
        0 => x"41",     -- A
        1 => x"79",     -- y
        2 => x"72",     -- r
        3 => x"74",     -- t
        4 => x"6F",     -- o
        5 => x"6E",     -- n
        6 => x"20",     --
        7 => x"41",     -- A
        8 => x"6C",     -- l
        9 => x"62",     -- b
        10 => x"75",    -- u
        11 => x"71",    -- q
        12 => x"75",    -- u
        13 => x"65",    -- e
        14 => x"72",    -- r
        15 => x"71",    -- q
        16 => x"75",    -- u
        17 => x"65",    -- e
        18 => x"20",    --
        19 => x"2B",    -- +
        20 => x"2B",    -- +
        21 => x"2B",    -- +
        22 => x"2D",    -- -
        23 => x"2D",    -- -
        24 => x"2B",    -- +
        25 => x"2B",    -- +
        26 => x"2B",    -- +
        27 => x"20"     --
    );
    
    component BRAM is
        port
        (
            reset      : IN std_logic ;
            clock      : IN std_logic ;
            wren       : IN std_logic ;
            rden       : IN std_logic ;
            chipselect : IN std_logic ;
            address    : IN std_logic_vector (9 downto 0);
            write       : IN std_logic_vector (7 downto 0);
            read       : OUT std_logic_vector (7 downto 0)
        );
    end component;
begin
    uut: BRAM port map(reset, clock, wren, rden, chipselect, address, write, read);

    process
    begin
        reset <= '1';
        wait for 15 ns;
        reset <= '0';
        wait;
    end process;

    process
    begin
        clock <= '0';
        wait for 10 ns;
        clock <= '1';
        wait for 10 ns;
    end process;
    
    process(reset, clock)
    begin
        if reset = '1' then
            state <= idle0;
        elsif clock'event and clock = '1' then
            counter <= counter + 1;
            case state is
                when idle0 =>
                    if counter = 10 then
                        state <= write0;
                        counter <= 0;
                    end if;
                when write0 =>
                    state <= write1;
                when write1 =>
                    index <= index + 1;
                    counter <= 0;
                    
                    if i = nome'length - 1 then
                        i <= 0;
                    else
                        i <= i + 1;
                    end if;
                    
                    if index >= 1024 then
                        index <= 0;
                        state <= idle1;
                    else
                        state <= idle0;
                    end if;
                when idle1 =>
                    state <= read0;
                    counter <= 0;
                when read0 =>
                    state <= read1;
                    
                    if counter = 10 then
                        state <= read0;
                        counter <= 0;
                    end if;
                when read1 =>
                    index <= index + 1;
                    state <= idle1;
                    counter <= 0;
                    
                    if index >= 1024 then
                        index <= 0;
                    end if;
            end case;
        end if;
    end process;

    process(state)
    begin
        case state is
            when idle0 =>
                write <= x"00";
                wren <= '0';
                rden <= '0';
                chipselect <= '0';
                address <= (others => '0');
            when write0 =>
                write <= nome(i);
                wren <= '1';
                rden <= '0';
                chipselect <= '1';
                address <= std_logic_vector(to_unsigned(index, address'length));
            when write1 =>
                write <= nome(i);
                wren <= '0';
                rden <= '0';
                chipselect <= '0';
                address <= std_logic_vector(to_unsigned(index, address'length));
            when idle1 =>
                write <= (others => '0');
                wren <= '0';
                rden <= '0';
                chipselect <= '0';
                address <= (others => '0');
            when read0 =>
                write <= x"00";
                wren <= '0';
                rden <= '1';
                chipselect <= '1';
                address <= std_logic_vector(to_unsigned(index, address'length));
            when read1 =>
                write <= x"00";
                wren <= '0';
                rden <= '0';
                chipselect <= '0';
                address <= std_logic_vector(to_unsigned(index, address'length));
        end case;
    end process;
end architecture;
