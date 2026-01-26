package main

import "fmt"

func main() {
    var arr = [10]int{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}
    slice := arr[1:4:5]
    fmt.Println(arr)
    fmt.Println(slice)

    slice[0] *= 8
    slice[3] = 5
    fmt.Println(arr)
    fmt.Println(slice)

    arr[3] += 10
    fmt.Println(arr)
    fmt.Println(slice)
}
