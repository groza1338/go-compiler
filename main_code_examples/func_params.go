package main

import "fmt"

func showAndMutate(i int, s string, arr [3]int) {
    fmt.Println("showAndMutate before inside: ", i, s, arr)
    i += 7
    s = s + "lang"
    arr[0] += 10
    arr[1] -= 1
    arr[2] *= 2
    fmt.Println("showAndMutate after inside: ", i, s, arr)
}

func showAndMutateRef(i *int, s *string, arr *[3]int) {
    fmt.Println("showAndMutateRef before inside: ", *i, *s, *arr)
    *i += 7
    *s = *s + "lang"
    (*arr)[0] += 10
    (*arr)[1] -= 1
    (*arr)[2] *= 2
    fmt.Println("showAndMutateRef after inside: ", *i, *s, *arr)
}

func main() {
    i := 5
    s := "go"
    arr := [3]int{1, 2, 3}

    showAndMutate(i, s, arr)
    fmt.Println("main after showAndMutate: ", i, s, arr)

    showAndMutateRef(&i, &s, &arr)
    fmt.Println("main after showAndMutateRef: ", i, s, arr)
}
