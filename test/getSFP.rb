#!/usr/bin/ruby

script_root = File.dirname(__FILE__)
bin_root = "#{script_root}/../build/bin"
ws = IO.readlines("#{script_root}/window_length")

for i in ws
    #system("#{bin_root}/SampledFootprintApproximation -t ~/trace/bzip2.001.trace -y #{i}")
    system("#{bin_root}/SampledFootprintApproximation -t ~/trace/mcf.001.trace -y #{i}")
end
