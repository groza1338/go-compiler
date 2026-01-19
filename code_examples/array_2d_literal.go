package main

import "fmt"

func main() {
	arr := [2][3]int{[3]int{1, 2, 3}, [3]int{4, 5, 6}}
	arr[0][2] = 33
	fmt.Print(arr)
	fmt.Print("\n")
	fmt.Print(arr[0])
	fmt.Print("\n")
	fmt.Print(arr[1])
	fmt.Print("\n")
	fmt.Print(arr[1][2])
}