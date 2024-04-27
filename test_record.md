Example中给出的方法，100个元素就无法通过精度检测：

```python
ValueError: For each axis slice, the sum of the observed frequencies must agree with the sum of the expected frequencies to a relative tolerance of 1e-08, but the percent differences are:
0.01
```

Example中方法（离散半高斯分布实现）：一次需要生成一个72比特的随机数，然后循环1～18次查表sample z。



按Example给出的精度(72bit)，实现[Pre17], 只需要10bit*18存储概率分布的信息。在10bit精度下，我们的方法也无法通过100个元素的精度检测(可进一步提高精度)





[Pre17]的方法: 先生成一个10比特的随机数，如果不满足条件还需要继续生成10比特的随机数， 则生成随机数的次数最大为10bit*18次。