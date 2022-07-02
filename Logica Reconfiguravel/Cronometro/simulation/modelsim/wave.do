vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cronometro/Frequencia.vhd}
vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cronometro/Contador.vhd}
vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cronometro/BCD.vhd}
vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cronometro/Botao.vhd}
vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cronometro/Cronometro.vhd}
vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cronometro/Cronometro_tb.vhd}
vsim +altera -do Cronometro_run_msim_rtl_vhdl.do -l msim_transcript -gui rtl_work.cronometro_tb
onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /cronometro_tb/uut/clock
add wave -noupdate /cronometro_tb/uut/tick
add wave -noupdate /cronometro_tb/uut/reset
add wave -noupdate /cronometro_tb/uut/enable
add wave -noupdate /cronometro_tb/uut/clear
add wave -noupdate /cronometro_tb/uut/uc
add wave -noupdate /cronometro_tb/uut/dc
add wave -noupdate /cronometro_tb/uut/us
add wave -noupdate /cronometro_tb/uut/ds
add wave -noupdate -radix unsigned /cronometro_tb/uut/uc_q
add wave -noupdate -radix unsigned /cronometro_tb/uut/dc_q
add wave -noupdate -radix unsigned /cronometro_tb/uut/us_q
add wave -noupdate -radix unsigned /cronometro_tb/uut/ds_q
add wave -noupdate /cronometro_tb/uut/uc_reset
add wave -noupdate /cronometro_tb/uut/dc_reset
add wave -noupdate /cronometro_tb/uut/us_reset
add wave -noupdate /cronometro_tb/uut/ds_reset
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {0 ps} 0}
quietly wave cursor active 0
configure wave -namecolwidth 188
configure wave -valuecolwidth 64
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {0 ps} {999 ps}
run 70 ms
