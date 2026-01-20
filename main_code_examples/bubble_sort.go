package main

import "fmt"

func main() {
	var n int
	fmt.Print("Enter array len: ")
	fmt.Scan(&n)
	if n > 10 || n < 1 {
	    fmt.Print("Incorrect len")
	    return
	}
	arr := [10]int{}
	for x := 0; x < n; x++ {
		fmt.Scan(&arr[x])
	}
	for i := 0; i < n; i++ {
		for j := 0; j < n-1; j++ {
			if arr[j] > arr[j+1] {
				tmp := arr[j]
				arr[j] = arr[j+1]
				arr[j+1] = tmp
			}
		}
	}

    fmt.Print("[")
	for i := 0; i < n; i++ {
		fmt.Print(arr[i])
		fmt.Print(" ")
	}
    fmt.Print("]")
}