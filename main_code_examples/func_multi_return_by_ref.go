package main

import "fmt"

func multiFuncRef(x_1 *int, x_2 *float64, x_3 *string, x_4 *rune, x_5 *bool) {
    *x_1 += 2
    *x_2 += 7.9
    *x_3 += "_new_part"
    *x_4 += 2
    *x_5 = !*x_5
}

func arrayInc(arr *[3]int) {
    (*arr)[0] += 10
    (*arr)[1] += 20
    (*arr)[2] += 30
}

func main() {
    var a int = 50
    var b float64 = 22.2
    var c string = "my_string"
    var d rune = 'a'
    var e bool = true
    var arr [3]int = [3]int{1, 2, 3}
    var arr_2 = [2][3]int{{1, 2, 3}, {4, 5, 6}}

    fmt.Print(a, " ", b, " ", c, " ", d, " ", e, "\n")
    multiFuncRef(&a, &b, &c, &d, &e)
    fmt.Print(a, " ", b, " ", c, " ", d, " ", e, "\n")
    fmt.Print(arr, "\n")
    arrayInc(&arr)
    fmt.Print(arr, "\n")
    fmt.Print(arr_2, "\n")
    arrayInc(&arr_2[0])
    arrayInc(&arr_2[1])
    fmt.Print(arr_2, "\n")


}
