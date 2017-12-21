#!/usr/bin/Rscript

require('ggplot2')
require('reshape')

args = commandArgs(trailingOnly = T)

dat = read.table(args[1], header=F)
colnames(dat) = c("feq", "count")

sr = as.numeric(args[2])

dat$feq = as.integer(dat$feq / (as.integer(sr/0.01)))
dat = dat[which(dat$feq > 10 & dat$feq < 750), ]

scount = sum(dat$count) * 1.0
dat$count = dat$count / scount * 100.0


dat = aggregate(. ~ feq, dat, sum)

plot = ggplot(dat)+
  geom_bar(stat="identity", aes(feq, count))

plot