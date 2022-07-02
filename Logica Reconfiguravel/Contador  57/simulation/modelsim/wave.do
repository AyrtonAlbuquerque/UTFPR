vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cont_57/Contador.vhd}
vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cont_57/Cont_57.vhd}
vcom -reportprogress 300 -work work {C:/Users/Ayrton/Documents/Quartus II/Cont_57/Cont_57_tb.vhd}
vsim +altera -do Cont_57_run_msim_rtl_vhdl.do -l msim_transcript -gui rtl_work.cont_57_tb
onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /cont_57_tb/clock
add wave -noupdate -radix unsigned /cont_57_tb/qu
add wave -noupdate -radix unsigned /cont_57_tb/qd
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {9999198 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 150
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
configure wave -timelineunits ns
update
WaveRestoreZoom {2359461 ps} {5012349 ps}
run 10000 ns
