package main

func main() {
	var grid [2][3]int

	grid[0][1] = 4
	grid[1][2] = grid[0][1] + 1

	value := grid[1][2]
	_ = value
}
