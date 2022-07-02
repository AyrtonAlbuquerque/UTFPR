vcom -reportprogress 300 -work work ./../../RAM.vhd
vcom -reportprogress 300 -work work ./../../BRAM.vhd
vcom -reportprogress 300 -work work ./../../BRAM_tb.vhd
vsim +altera -do BRAM_run_msim_rtl_vhdl.do -l msim_transcript -gui rtl_work.bram_tb
onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /bram_tb/reset
add wave -noupdate /bram_tb/clock
add wave -noupdate /bram_tb/wren
add wave -noupdate /bram_tb/rden
add wave -noupdate /bram_tb/chipselect
add wave -noupdate /bram_tb/write
add wave -noupdate -radix ascii /bram_tb/read
add wave -noupdate /bram_tb/address
add wave -noupdate /bram_tb/counter
add wave -noupdate /bram_tb/index
add wave -noupdate /bram_tb/i
add wave -noupdate /bram_tb/state
add wave -noupdate -radix ascii /bram_tb/nome
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {268207972 ps} 0}
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
configure wave -timelineunits ps
update
WaveRestoreZoom {268206203 ps} {271082087 ps}
run 1000000 ns
