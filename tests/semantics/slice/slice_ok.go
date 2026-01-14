package main

func main() {
	arr := [3]int{1, 2, 3}
	_ = arr[1:]
	_ = arr[:2]
	s := arr[0:2]
	_ = s
}
