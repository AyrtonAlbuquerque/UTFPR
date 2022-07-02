vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/BitCounter/simulation/modelsim/BitCounter.vho}
vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/BitCounter/BitCounter_tb.vhd}
vsim +altera -do BitCounter_run_msim_gate_vhdl.do -l msim_transcript -gui -sdftyp {/uut=C:/Users/Ayrton/Documents/Quartus II/BitCounter/simulation/modelsim/BitCounter_vhd.sdo} work.bitcounter_tb
onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /bitcounter_tb/clock
add wave -noupdate /bitcounter_tb/reset
add wave -noupdate -radix unsigned /bitcounter_tb/variavel
add wave -noupdate -radix unsigned /bitcounter_tb/sinal_for
add wave -noupdate -radix unsigned /bitcounter_tb/sinal_while
add wave -noupdate /bitcounter_tb/entrada
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {0 ps} 0}
quietly wave cursor active 0
configure wave -namecolwidth 179
configure wave -valuecolwidth 100
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
WaveRestoreZoom {0 ps} {982 ps}
run 500 ns
