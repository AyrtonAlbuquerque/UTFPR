library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Semaphore is
    port(
        clock     : in  std_logic;
        reset     : in  std_logic;
        enable    : in  std_logic;
        reg0      : in  std_logic_vector(31 downto 0);
        reg1      : in  std_logic_vector(31 downto 0);
        reg2      : in  std_logic_vector(31 downto 0);
        reg3      : in  std_logic_vector(31 downto 0);
        remaining : out std_logic_vector(15 downto 0);
        status    : out std_logic_vector(3 downto 0)
    );
end entity;

architecture behavioral of Semaphore is
    type states is (IDLE, GREEN_RED, YELLOW_RED, RED_RED, RED_GREEN, RED_YELLOW, RED_RED2);

    signal state, next_state : states;
    signal count, counter    : integer;
    signal tick              : std_logic;

    component Timer
        generic(period : integer := 1);
        port(
            clock  : in  std_logic;
            reset  : in  std_logic;
            enable : in  std_logic;
            tick   : out std_logic
        );
    end component;
begin
    sys_tick : Timer
        generic map(1)
        port map(clock, reset, enable, tick);

    status    <= std_logic_vector(to_unsigned(states'pos(state), status'length));
    remaining <= std_logic_vector(to_unsigned(counter, remaining'length));

    process(reset, clock)
    begin
        if reset = '0' then
            state   <= IDLE;
            counter <= 0;
        elsif rising_edge(clock) then
            if enable = '1' and tick = '1' then
                if state /= next_state then
                    counter <= 0;
                else
                    counter <= counter + 1;
                end if;

                state <= next_state;
            end if;
        end if;
    end process;

    process(state, counter, count)
    begin
        case state is
            when IDLE =>
                count <= 1;

                if counter = count then
                    next_state <= states'val(to_integer(unsigned(reg3(23 downto 20))));
                else
                    next_state <= state;
                end if;
            when GREEN_RED =>
                count <= to_integer(unsigned(reg0(15 downto 0))) * 1000;

                if counter = count then
                    next_state <= YELLOW_RED;
                else
                    next_state <= state;
                end if;
            when YELLOW_RED =>
                count <= to_integer(unsigned(reg1(15 downto 0))) * 1000;

                if counter = count then
                    next_state <= RED_RED;
                else
                    next_state <= state;
                end if;
            when RED_RED =>
                count <= to_integer(unsigned(reg2(15 downto 0))) * 1000;

                if counter = count then
                    next_state <= RED_GREEN;
                else
                    next_state <= state;
                end if;
            when RED_GREEN =>
                count <= to_integer(unsigned(reg0(15 downto 0))) * 1000;

                if counter = count then
                    next_state <= RED_YELLOW;
                else
                    next_state <= state;
                end if;
            when RED_YELLOW =>
                count <= to_integer(unsigned(reg1(15 downto 0))) * 1000;

                if counter = count then
                    next_state <= RED_RED2;
                else
                    next_state <= state;
                end if;
            when RED_RED2 =>
                count <= to_integer(unsigned(reg2(15 downto 0))) * 1000;

                if counter = count then
                    next_state <= GREEN_RED;
                else
                    next_state <= state;
                end if;
        end case;
    end process;
end architecture;

