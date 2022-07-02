library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity Cronometro is
    port(
        clock  	: in std_logic;
        reset  	: in std_logic;
        enable 	: in std_logic;
		clear   : in std_logic;
        uc     	: out std_logic_vector (6 downto 0);
        dc     	: out std_logic_vector (6 downto 0);
        us     	: out std_logic_vector (6 downto 0);
        ds     	: out std_logic_vector (6 downto 0)
    );
end entity;

architecture behavioral of Cronometro is
    constant started : std_logic := '1';
	constant stopped : std_logic := '0';

    signal enable_state, clear_state, tick : std_logic;
    signal uc_reset, dc_reset, us_reset, ds_reset : std_logic;
    signal uc_q, dc_q, us_q, ds_q : std_logic_vector (3 downto 0);

    component Frequencia is
        generic (periodo : integer := 1);

        port
        (
            sinal   : IN std_logic ;
            reset   : IN std_logic ;
            clock   : OUT std_logic
        );
    end component;

    component Contador is
        port
        (
            clock  : IN std_logic ;
            reset  : IN std_logic ;
            enable : IN std_logic ;
            q      : OUT std_logic_vector (3 downto 0)
        );
    end component;

    component BCD is
        port
        (
            entrada : IN std_logic_vector (3 downto 0);
            saida   : OUT std_logic_vector (6 downto 0)
        );
    end component;

    component Botao is
        generic (periodo : integer := 30);

        port
        (
            clock   : IN std_logic ;
			reset	: IN std_logic ;
			hold	: IN std_logic ;
            entrada : IN std_logic ;
            saida   : OUT std_logic
        );
    end component;
begin
    sys_clock: Frequencia generic map(periodo => 1) port map(clock, reset, tick);

    btn_clear: Botao generic map(periodo => 30) port map(tick, reset, '0', clear, clear_state);
    btn_enable: Botao generic map(periodo => 30) port map(tick, reset, '1', enable, enable_state);

    cont_uc: Contador port map(tick, uc_reset, enable_state, uc_q);
    bcd_uc: BCD port map(uc_q, uc);

    cont_dc: Contador port map(uc_reset, dc_reset, enable_state, dc_q);
    bcd_dc: BCD port map(dc_q, dc);

    cont_us: Contador port map(dc_reset, us_reset, enable_state, us_q);
    bcd_us: BCD port map(us_q, us);

    cont_ds: Contador port map(us_reset, ds_reset, enable_state, ds_q);
    bcd_ds: BCD port map(ds_q, ds);
	
    uc_reset <= '1' when ((uc_q = "1010") or (clear_state = '1' and enable_state = stopped) or reset = '1') else '0';
    dc_reset <= '1' when ((dc_q = "1010") or (clear_state = '1' and enable_state = stopped) or reset = '1') else '0';
    us_reset <= '1' when ((us_q = "1010") or (clear_state = '1' and enable_state = stopped) or reset = '1') else '0';
    ds_reset <= '1' when ((ds_q = "0110") or (clear_state = '1' and enable_state = stopped) or reset = '1') else '0';
end architecture;
