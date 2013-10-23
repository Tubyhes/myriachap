/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/


/**
 * \file myriaChap.h
 *
 *	\mainpage
 *
 *	\par Introduction
 *	MyriaChap is a platform for creating evolving and evolvable applications. The main functionality is that it allows
 *	applications to change their behavior at runtime. This is achieved by breaking down applications into small blocks,
 *	called components, each implementing a basic function. These components are then combined by connecting them through
 *	channels, so as to create more complex behavior. Components can be created, deleted and recombined at runtime.
 *
 *
 *	\par Rationale
 *	There are two main reasons behind the MyriaChap approach.\n
 *	The first is pattern detection without any a priori knowledge on what the pattern might look like. WSN's typically
 *	are used for observing an environment and detecting anomalies using sensors. However, what an anomaly is depends on
 *	the type of sensor, the environment, the people (if any) in the environment, etc. To prevent every single WSN
 *	deployment from being an extensive project that includes recording exactly what patterns the network might encounter,
 *	it is important that the network is able to learn itself what patterns are in the environment. This requires the
 *	network to search for such patterns, which may be of different temporal scales, different modalities and distributed
 *	over the environment. As the nodes in a WSN are too limited to perform an exhaustive search in all these dimensions,
 *	nodes are basically required to constantly alter their behavior. \n
 *	The second reason is autonomous operation. As the goal with WSN's is typically to leave them both operational and
 *	unattended in an environment for several years, the WSN must have substantial self-diagnosis and self-repair
 *	capabilities. On the application level, this involves this involves nodes identifying buggy operations, operations
 *	that are too time- or energy consuming, but also to act on this, and to warn other nodes of this. Also, it is
 *	very important that a node will never fail and stop working through faulty code. Thus, there should always be a
 *	background process that evaluates ongoing operations and can intervene if something goes wrong, without interfering
 *	with correctly executing operations. \n
 *
 * \author Almende B.V. - Freek van Polen
 * \version 0.5
 */

#ifndef MYRIACHAP_H_
#define MYRIACHAP_H_

#include "../../scheduler/interfaces/interfaces.h"
#include "structures.h"
#include "boxStorage.h"
#include "chapStdlib.h"
#include "myriaChapOut.h"

#endif /* MYRIACHAP_H_ */
