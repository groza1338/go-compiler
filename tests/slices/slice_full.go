package main

func main() {
	var nums [4]int

	nums[0] = 1
	nums[1] = 2

	slice := nums[:]
	slice[1] = nums[0]

	_ = slice[2]
}
