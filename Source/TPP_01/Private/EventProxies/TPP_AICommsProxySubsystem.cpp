#include "TPP_01/Public/EventProxies/TPP_AICommsProxySubsystem.h"

void UTPP_AICommsProxySubsystem::NotifyAICommEvent(const class UTPP_AICommsEventBase* AICommEvent) { OnAICommEventTriggered.Broadcast(AICommEvent); }
