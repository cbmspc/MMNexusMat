function AcqData = nexus_getdataff (AcqSamplRateHz, AcqTimeSeconds, NumChannels)
AcqData = nexus_getdata(AcqSamplRateHz, AcqTimeSeconds, NumChannels, [], 1);
