package main

import (
	"fmt"
	"sort"
)

var farmiliar map[int]([]int)

func f() {
	fmt.Println("")
}

//use GetDistSecondVersion

func GetFarmiliar() {
	for i:=0;i<clustnum;i++ {
		clust:=ClustArr[i]
		for stud1,_:=range clust.clust_set.mp {
			for stud2,_:=range clust.clust_set.mp {
				if stud1 == stud2 {
					continue
				}
				farmiliar[stud1] = append(farmiliar[stud1], stud2)
			}
			sort.Ints(farmiliar[stud1])
		}
	}
}

