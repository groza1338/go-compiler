package main

import "fmt"

func main() {
    arr := [5]int{5, 1, 4, 2, 3}
    n := 5
    for i := 0; i < n; i++ {
        for j := 0; j < n-1; j++ {
            if arr[j] > arr[j+1] {
                tmp := arr[j]
                arr[j] = arr[j+1]
                arr[j+1] = tmp
            }
        }
    }
    fmt.Print(arr)
}
