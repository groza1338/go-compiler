package main

func main() {
	var values [3]int

	values[0] = 10
	values[1] = values[0] - 1

	v := values[2]
	_ = v
}
