#程序一
str = input("list1:") 
list1 = [int(i) for i in str.split()]
print(len(list1))
for i in range(0,len(list1)) :
    print(list1[i])
for i in range(-10,10) :
    if i<0 :
        print (i,"is a negetive number!")
    elif i&1 :
        print (i,"is a odd number!")
    else :
        print (i,"is a even number!")
print("hello world!")
A="I"
B="am"
C="overload1997!"
print(A,B,C,end=" ")

#程序二
#列表
list = [1,2,3,4,5,6,1]
print(list)
#元组
tup = (1,2,3,4,5,6,1)
print(tup)
#集合
    #可以使用大括号 { } 或者 set() 函数创建集合，注意：
    #创建一个空集合必须用 set() 而不是 { }，因为 { } 是用来创建一个空字典。
student = {'Tom', 'Jim', 'Mary', 'Tom', 'Jack', 'Rose'}
print(student)  # 输出集合，重复的元素被自动去掉
a = set('abracadabra')
b = set('alacazam')
print(a&b)
print(a|b)
print(a^b)
print(b^a)
print(a)
#字典(C++中的map)
    #这样用有点恶心
dic = dict(overload=1997 , aaa="文明宿舍" , tencent="QQ")
print(dic)
    #还是参照C++吧
dic1 = {} #创建空的字典用 {}
dic1["露娜"]="打野"
dic1["后裔"]="ADC"
dic1["白起"]="上单"
print(dic1)
print(dic1["露娜"])
