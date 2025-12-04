package main

func main() {
	var data [5]int

	data[1] = 3
	data[3] = 7

	var mid []int
	mid = data[1:4]

	tail := mid[1:]
	_ = tail
}
