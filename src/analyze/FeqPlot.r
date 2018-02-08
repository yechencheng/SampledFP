#!/usr/bin/Rscript

require('ggplot2')
require('reshape')

args = commandArgs(trailingOnly = T)

dat = read.table(args[1], header=F)

output = "output.pdf"
if(length(args) >= 2)
  output = args[2]

caption = ""
if(length(args) >= 3)
  caption = args[3]

colnames(dat) = c("feq", "count")

#dat = dat[which(dat$feq > 10), ]
scount = sum(as.numeric(dat$count)) * 1.0
dat$count = dat$count / scount

dat = aggregate(. ~ feq, dat, sum)

plot = ggplot(dat)+
  geom_bar(stat="identity", aes(feq, count))+
  scale_y_continuous(labels = scales::percent)+
  labs(x = "Frequency", y="Ratio")

ggsave(output)
