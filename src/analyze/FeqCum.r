#!/usr/bin/Rscript

require('ggplot2')
require('reshape')

args = commandArgs(trailingOnly = T)
dat = read.table(args[1], header=F)

colnames(dat) = c("feq", "count")
dat$cnt_ratio = dat$count / sum(as.numeric(dat$count))
dat$cum_ratio = cumsum(dat$cnt_ratio)
dat = dat[which(dat$feq > 4), ]

print(dat)
plot = ggplot(dat)+
    geom_bar(stat="identity", aes(feq,cnt_ratio))+
    scale_y_continuous(labels = scales::percent)

plot