// Fill out your copyright notice in the Description page of Project Settings.


#include "Flow.h"

Flow::Flow()
{
}

Flow::~Flow()
{
}

FFlowData Flow::Create()
{
    FFlowData data;
    data.SourceIPAddress = "10.0.0.1";
    data.DestinationIPAddress = "10.0.0.2";
    data.MeanDelay = 0;
    data.MeanJitter = 0;
    data.TxBytes = 0;
    data.TxPackets = 0;
    data.RxBytes = 0;
    data.RxPackets = 0;
    data.PacketLossL3 = 0;
    return data;
}
