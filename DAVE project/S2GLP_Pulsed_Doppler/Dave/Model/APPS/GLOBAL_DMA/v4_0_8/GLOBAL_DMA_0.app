<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="GLOBAL_DMA" URI="http://resources/4.0.8/app/GLOBAL_DMA/0" description="The GLOBAL_DMA APP initializes the DMA module,&#xA; sets channel priorities and reserves a DMA&#xA; interrupt node for handling GPDMA events." version="4.0.8" minDaveVersion="4.0.0" instanceLabel="GLOBAL_DMA_0" appLabel="">
  <upwardMapList xsi:type="ResourceModel:RequiredApp" href="../../DMA_CH/v4_1_8/DMA_CH_0.app#//@requiredApps.0"/>
  <upwardMapList xsi:type="ResourceModel:RequiredApp" href="../../DMA_CH/v4_1_8/DMA_CH_2.app#//@requiredApps.0"/>
  <upwardMapList xsi:type="ResourceModel:RequiredApp" href="../../DMA_CH/v4_1_8/DMA_CH_5.app#//@requiredApps.0"/>
  <upwardMapList xsi:type="ResourceModel:RequiredApp" href="../../DMA_CH/v4_1_8/DMA_CH_6.app#//@requiredApps.0"/>
  <properties sharable="true"/>
  <virtualSignals name="Interrupt signal" URI="http://resources/4.0.8/app/GLOBAL_DMA/0/interrupt_dma" hwSignal="signal_in" hwResource="//@hwResources.0">
    <upwardMapList xsi:type="ResourceModel:Connections" href="../../DMA_CH/v4_1_8/DMA_CH_0.app#//@connections.0"/>
    <upwardMapList xsi:type="ResourceModel:Connections" href="../../DMA_CH/v4_1_8/DMA_CH_2.app#//@connections.0"/>
    <upwardMapList xsi:type="ResourceModel:Connections" href="../../DMA_CH/v4_1_8/DMA_CH_5.app#//@connections.0"/>
    <upwardMapList xsi:type="ResourceModel:Connections" href="../../DMA_CH/v4_1_8/DMA_CH_6.app#//@connections.0"/>
  </virtualSignals>
  <requiredApps URI="http://resources/4.0.8/app/GLOBAL_DMA/0/ra_cpu" requiredAppName="CPU_CTRL_XMC4" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../CPU_CTRL_XMC4/v4_0_14/CPU_CTRL_XMC4_0.app#/"/>
  </requiredApps>
  <hwResources name="NVIC DMA Node" URI="http://resources/4.0.8/app/GLOBAL_DMA/0/rg_nvicnodedma" resourceGroupUri="peripheral/cpu/0/nvic/interrupt/sv0" solverVariable="true" mResGrpUri="peripheral/cpu/0/nvic/interrupt/sv0">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/cpu/cpu_0.dd#//@provided.21"/>
    <solverVarMap index="5">
      <value variableName="sv0" solverValue="110"/>
    </solverVarMap>
    <solverVarMap index="5">
      <value variableName="sv0" solverValue="110"/>
    </solverVarMap>
  </hwResources>
</ResourceModel:App>
