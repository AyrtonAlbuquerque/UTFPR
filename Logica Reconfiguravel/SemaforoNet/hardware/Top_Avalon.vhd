library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Top_Avalon is
    port(
        clock      : in  std_logic;
        reset      : in  std_logic;
        wren       : in  std_logic;
        rden       : in  std_logic;
        chipselect : in  std_logic;
        address    : in  std_logic_vector(1 downto 0);
        write      : in  std_logic_vector(31 downto 0);
        read       : out std_logic_vector(31 downto 0)
    );
end entity;

architecture behavioral of Top_Avalon is
    signal wren0, wren1, wren2, wren3, enable         : std_logic;
    signal q0, q1, q2, q3, read0, read1, read2, read3 : std_logic_vector(31 downto 0);
    signal remaining                                  : std_logic_vector(15 downto 0);
    signal status, initial                            : std_logic_vector(3 downto 0);

    component Reg
        port(
            clock : in  std_logic;
            reset : in  std_logic;
            wren  : in  std_logic;
            d     : in  std_logic_vector(31 downto 0);
            q     : out std_logic_vector(31 downto 0)
        );
    end component;

    component Semaphore
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
    end component;
begin
    Reg0 : Reg port map(clock, reset, wren0, write, q0);
    Reg1 : Reg port map(clock, reset, wren1, write, q1);
    Reg2 : Reg port map(clock, reset, wren2, write, q2);
    Reg3 : Reg port map(clock, reset, wren3, write, q3);
    Sinaleiro : Semaphore port map(clock, reset, enable, q0, q1, q2, q3, remaining, status);

    enable  <= q3(24);
    initial <= q3(23 downto 20);

    wren0 <= '1' when wren = '1' and chipselect = '1' and address = "00" else '0';
    wren1 <= '1' when wren = '1' and chipselect = '1' and address = "01" else '0';
    wren2 <= '1' when wren = '1' and chipselect = '1' and address = "10" else '0';
    wren3 <= '1' when wren = '1' and chipselect = '1' and address = "11" else '0';

    -- Register 0: (15 downto 0) = Green count.
    read0 <= (31 downto 16 => '0') & q0(15 downto 0);
    -- Register 1: (15 downto 0) = Yellow count.
    read1 <= (31 downto 16 => '0') & q1(15 downto 0);
    -- Register 2: (15 downto 0) = Red count.
    read2 <= (31 downto 16 => '0') & q2(15 downto 0);
    -- Register 3: (24) = Start & Stop | (23 downto 20) = Initial state | (19 downto 16) = Current state | (15 downto 0) = Remaining count.
    read3 <= (31 downto 25 => '0') & enable & initial & status & remaining;

    read <= read0 when rden = '1' and chipselect = '1' and address = "00" else
            read1 when rden = '1' and chipselect = '1' and address = "01" else
            read2 when rden = '1' and chipselect = '1' and address = "10" else
            read3 when rden = '1' and chipselect = '1' and address = "11" else
            (others => '0');
end architecture;
