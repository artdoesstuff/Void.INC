#pragma once

#include "Globals/GIncludes.hpp"

inline bool logsUnlocked = false;
inline std::string logInput = "";
inline int selectedLog = 0;

struct LoreEntry {
    std::string title;
    std::string cycle;
    std::string content;
};

inline std::vector<LoreEntry> loreEntries = {
    {
        "GALACTIC SYSTEM V-1D", "56Y-36CY",
        "[ START OF TRANSMISSION ]\n\n"
        "LR. Taki: This is Lead Researcher Taki reporting to Void.INC in U-01-A. It has been 216 cycles since our departure, and we have yet to find anything noteworthy.\n\n"
        "Dir. Nexus: Copy that, Taki. Please hold the sector position and wait for further instructions.\n\n"
		"LR. Taki: Understood, Director. Will hold position in Sector 2473-C and will run scans in the meantime.\n\n"
		"[ TRANSMISSION ON HOLD ]\n\n"
        "LR. Taki: Director, we're picking up a system in Sector 2589-A. Relaying data back to HQ. Your orders, Director?\n\n"
        "Dir. Nexus: ... Feel free to explore, Taki. We'll be trying to establish a connection to a potential settlement.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "Dir. Nexus: We've established a connection, Taki. The system is called 'V-1D' by its inhabitants. Life in the system does not exist outside of 'V-1D-10'.\n\n"
        "LR. Taki: Understood, Director. Is there anything else we should know before we change course to pass by 'V-1D-10'?\n\n"
        "Dir. Nexus: Negative, Taki. We will be uploading new parameters to your terminal shortly. For now, please set course to 'V-1D-10'.\n\n"
		"LR. Taki: Acknowledged, Director. Setting course to 'V-1D-10' now.\n\n"
		"[ END OF TRANSMISSION ]"
    },
    {
        "RECALIBRATION", "56Y-39CY",
        "[ START OF TRANSMISSION ]\n\n"
        "LR. Taki: Greetings, this is Lead Researcher Taki reporting, again. I am requesting a terminal recalibration since our sensors have been going haywire.\n\n"
        "...: Please hold while we connect you to the appropriate department.\n\n"
        "LR. Taki: ... Stupid automated systems... Lead Researcher and I still can't get priority.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "RA. Tessa: Hello, Taki. This is Research Assis-\n\n"
        "LR. Taki: You're joking, right? A Research Assistant is the one- you know what, forget it. Just forward my request to recalibrate every terminal.\n\n"
        "RA. Tessa: I'm sorry, I don't quite follo-\n\n"
        "LR. Taki: JUST DO IT! GOSH! CAN I HAVE SOMEONE MORE COMPETENT!?\n\n"
        "RA. Tessa: Yes, Sir. I will forward your request immediately. I apologise for the inconvenience I may have caused you.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "Eng. Ryu: Hello, Destiny-4514. We have received your sensor data and have issued a firmware update to all terminals to reflect this change.\n\n"
        "LR. Taki: Thank you, U-01-A.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "LR. Taki: ... Sorry, what exactly am I looking at here?\n\n"
        "Eng. Ryu: A gravity well. Your data suggests that whatever you found is potentially a Star. Do you have any more requests?\n\n"
        "LR. Taki: ... Negative. Thank you, U-01-A. Just forward our progress to the Director.\n\n"
        "[ END OF TRANSMISSION ]"
    },
    {
        "SYSTEM V-1D-10", "56Y-47CY",
        "[ START OF RECORDING ]\n\n"
        "[XX:22] Pil. Erwin: Approaching 'V-1D-10'. Setting up shield rays, can never be too careful in an unknown system.\n\n"
        "[XX:23] Eng. Willius: Copy. Systems are optimal, and sensors aren't detecting anything unusual. Have you noticed anything off about Taki, though?\n\n"
        "[XX:23] Pil. Erwin: Can't say I haven't. After the terminals got updated, he looked like he had seen a ghost the entire time.\n\n"
        "[XX:24] Eng. Willius: Right? I don't know, something is off with this situation...\n\n"
		"[ RECORDING PAUSED ]\n\n"
        "[XX:32] Pil. Erwin: Hey, Taki. Are you doing okay? You look a little quesy.\n\n"
        "[XX:32] LR. Taki: I'm fine... just a little feverish.\n\n"
        "[XX:33] Eng. Willius: Taki, if there's something wrong, we should know. You're putting the crew at risk here.\n\n"
        "[XX:33] LR. Taki: You're one to talk. Leave me alone, why don't you?\n\n"
        "[XX:33] Eng. Willius: Taki, seriously. Did you see something \n\n"
        "[XX:34] LR. Taki: Just leave me alone. I'll be in th-\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:38] Eng. Willius: DOC! TAKI BLACKED OUT AND FELL DOWN A FLIGHT OF STAIRS!\n\n"
        "[XX:43] Dr. Iris: Well... goodness me. Is there anything else you'd want to tell me? His psychological condition is significantly worse.\n\n"
        "[XX:43] Eng. Willius: I... I'm not sure. He said he's feverish.\n\n"
        "[XX:44] Dr. Iris: ... On this shuttle? Oh no... Contact HQ immediately and tell them we have a problem. Request for emergency protocols for everyone.\n\n"
        "[XX:45] Eng. Willius: Is something going on, Doc?\n\n"
        "[XX:47] Dr. Iris: I don't have time to explain, Willius. I just need you to contact HQ as soon as possible. We may be running out of time.\n\n"
        "[ END OF RECORDING ]"
    },
    {
        "URGENT EMERGENCY #1", "56Y-48CY",
        "[ START OF TRANSMISSION ]\n\n"
        "Eng. Willus: Director Nexus, or whoever has Omega-level clearance, I am requesting that emergency protocols be deployed immediately on the shuttle, Destiny-4514.\n"
        "Something isn't right... Something is infecting our systems and crew at a rapid pace.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "Eng. Willus: Taki... Taki is infected, and his condition is the most critical. Some of the crew also claim they're beginning to hear voices as we get closer to 'V-1D-10'.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "Eng. Willus: We currently have 16 crewmates in the medbay who all share the same, or similar, irregular symptoms.\n"
        "We will await your orders, Director, but I must suggest that we either lock the infected crew off or have them ejected from the shuttle.\n"
        "We cannot risk our remaining crew to some unknown infection.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "Eng. Ryu: Ryu here, our current emergency protocols are unable to aid in the situation you just provided. Please give us at least 2 cycles to revise them.\n\n"
        "Eng. Willius: With all due resp-\n\n"
        "Eng. Ryu: You will wait. This is not to be negotiated.\n\n"
        "[ END OF TRANSMISSION ]"
    },
    {
        "E-PROTOCOL: REV021", "56Y-50CY",
        "[ START OF RECORDING ]\n\n"
        "[XX:37] Eng. Ryu: Ryu. We have pushed firmware update 1.0.5, designated 'neutraliser'. The goal is just to keep systems stable for the meantime as we continue researching this\n"
        "potential \"threat\" further. I will let the Secretary take over for the rest of what needs to be said.\n\n"
        "[XX:39] Sec. Rose: As per the revision and the requests of the following: Director Nexus, Engineer Willius, and Engineer Ryu; the Board has called to revise our emergency protocols\n"
        "to prevent a potential catastrophic event. We at Void.INC are tryi-\n\n"
        "[XX:40] Eng. Ryu: Save the 'corporate' talk for the applicants. To the researchers and engineers aboard Destiny-4514, currently, you have been cleared to eject the ill only if they are\n"
        "already deceased. Else, quarantine procedures are as follows: Lock them in Docking Bay #2C and keep them there, drop temperatures to negative 5 degrees celsius, and monitor them.\n"
        "However, Taki is to be kept in the medbay to allow Dr. Iris to report his condition to us at all times. This is non-negotiable. Thank you for your understanding, and good luck.\n\n"
        "[ END OF RECORDING ]"
    },
    {
        "PATIENT-ZERO", "56Y-51CY",
        "[ START OF TRANSMISSION ]\n\n"
        "Dr. Iris: I must say, I am quite concerned about the current condition of Taki, Ryu.\n\n"
        "Eng. Ryu: Get to the point, I'm not interested in small talk.\n\n"
        "Dr. Iris: Of course, apologies. Taki is slowly getting burned from within; his internal body temperature is already 50 degrees celsius. Along with thi-\n\n"
        "Eng. Ryu: And he's not dead yet? Any other findings, then?\n\n"
        "Dr. Iris: Yes... as I was saying, along with this, his blood is becoming more viscous and has already turned a deep black...\n\n"
        "Eng. Ryu: Anything else? You sound hesitant to say something.\n\n"
        "Dr. Iris: ... The blood is becoming senti-\n\n"
        "Eng. Ryu: Eject him immediately. Let HQ deal with the aftermath.\n\n"
        "Dr. Iris: Ryu, I implore you to reconsi-\n\n"
        "Eng. Ryu: Do it. This is not a request. It is an order.\n\n"
        "[ END OF TRANSMISSION ]"
    },
    {
        "E-PROTOCOL: REV022", "56Y-54CY",
        "[ START OF RECORDING ]\n\n"
        "[XX:19] Eng. Ryu: Ryu, again. I personally pushed firmware update 1.0.6 despite the dismissal and refusal of the Board. Systems are more stable now, but emergency protocols are still\n"
        "in place and revised again. The \"threat\" is a biological entity residing in the 'V-1D' system. I've dubbed this entity 'Archangel', but Corporate did not like that.\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:23] Eng. Ryu: Moving on, most of our R&D departments are focused on researching this entity, and I am certainly not liking the results gathered thus far...\n"
        "Slight change in parameters for those aboard Destiny-4514: Eject all ill. Do not allow them to mingle with any healthy personnel.\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:26] Eng. Ryu: Director Nexus was a moron for sending Destiny-4514 to an unknown system instead of dron- I see. That's certainly interesting... So you can see and hear me.\n"
        "Must suck to know I destroy everything I use.\n\n"
        "[ ERROR: RECORDING CORRUPTED ]"
    },
    {
        "ALERT: VICTIM", "56Y-55CY",
        "[ START OF RECORDING ]\n\n"
        "[XX:00] Dir. Nexus: It has come to the attention of Void.INC that our Lead Researcher, Taki Elis, was recently deceased due to ejection into space, due to the orders of one of our\n"
        "engineers, Ryu Saito. Ryu is now wanted by the Board for treaso-\n\n"
        "[XX:02] Sec. Rose: Apologies, Sir. But I do have to tell you to reconsider. Ryu is currently leading our efforts to stop this \"threat\" that the Board is blatantly ignoring.\n"
        "I suggest we just mention Taki's death, but leave out Ryu's participation. We don't want to cause panic or to let Ryu believe he is the enemy.\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:30] Sec. Rose: It is an unfortunate day for us all. Aboard the shuttle, Destiny-4514, Lead Research Taki had finally succumbed to whatever infection he had contracted while in\n"
        "transit to 'V-1D-10'. His mission parameters have already been transferred over to another Researcher following his death.\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:32] Sec. Rose: We at Void.INC cannot stress enough how important it is to follow protocol at this time. We do not want this to spread further until we understand exactly what it is\n"
        "we're up against. All applicants are to proceed with more caution because 37 applicants have already been terminated.\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:49] Dir. Nexus: Find Ryu and bring him to me. I will not allow a rogue to be the one handling our systems until I know we're on the same page.\n\n"
        "[XX:50] Sec. Rose: ... Uh... Understood, Sir.\n\n"
        "[ END OF RECORDING ]"
    },
    {
        "CONTAINMENT ATT1", "56Y-58CY",
        "[ START OF RECORDING ]\n\n"
        "[XX:14] Jr. Eng. Lu: Uhm... is this thing on? Okay. Hello, I am Junior Engineer Lu. I am taking over temporarily because Ryu was supposedly \"terminated\" from his position.\n"
        "I have added a temporary containment field in the terminal's firmware to \"negate\" its influence...\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:16] Jr. Eng. Lu: ... I doubt it'll actually work after reviewing Ryu's hidden logs. Corporate's ideas are always hit or miss. Uhm... good luck to the remaining 61 applicants;\n"
        "your numbers have already dwindled significantly.\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:42] Jr. Eng. Lu: ... Haha... well... I see why Ryu calls you 'Archangel'. You are truly beyond our understanding. Well done for infecting me, but I promise you... Ryu will win.\n"
        "He won't lose... not to yo-\n\n"
        "[ ERROR: RECORDING CORRUPTED ]"
    },
    {
        "E-PROTOCOL: OFFLINE", "56Y-63CY",
        "[ START OF RECORDING ]\n\n"
        "[XX:32] Sec. Rose: ... Director, we are unable to find Ryu.\n\n"
        "[XX:32] Dir. Nexus: What? What do you mean you were unable to find Ryu?\n\n"
        "[XX:33] Sec. Rose: He's... off the system. All of our high-ranking personnel are locked out of the terminals. The... the other engineers and researchers are not responding to their\n"
        "pagers either.\n\n"
        "[XX:35] Dir. Nexus: ... Are you implying that all of them are now working for him?\n\n"
        "[XX:36] Sec. Rose: Hard to say for sure... his tracker is also inactive, we don't know where he is. We also fou-\n\n"
        "[ ERROR: RECORDING CORRUPTED ]\n\n"
        "[ START OF RECORDING ]\n\n"
        "[??:??] ???: I would like to apologise for the previous log writer's doubts and pessimism; he has been terminated as a result. Emergency protocols have also been lifted at this time,\n"
        "so all previous parameters and protocols can be dismissed. You are safe...\nYou are safe...\nYou are safe...\nY0U.\n4R3.\nS4F3.\n\n"
        "[ END OF RECORDING ]"
    },
    {
        "TRANSMISSION 5703XAA", "57Y-03CY",
        "[ START OF TRANSMISSION ]\n\n"
        "Eng. Ryu: Ryu contacting Destiny-4514. How is your current status?\n\n"
        "Eng. Willius: ... Willius responding. Most of our crew has been ejected as per your orders... including Dr. Iris.\n\n"
        "Eng. Ryu: Numbers. I don't need names.\n\n"
        "Eng. Willius: Is that all you care about!? Numbers and data instea- sorry... right. 38 ejected, 4 confirmed deceased, 7 healthy.\n\n"
        "Eng. Ryu: Is that so? Well- ... stop transmitting.\n\n"
        "[ END OF TRANSMISSION ]\n\n"
        "[ START OF TRANSMISSION ]\n\n"
        "Eng. Ryu: It was listening. Very little time to explain; eject your remaining crew until it's just you and Researcher Eris left. Those \"healthy\" are already infected.\n\n"
        "Eng. Willius: ... Could you repeat th-\n\n"
        "[ END OF TRANSMISSION ]"
    },
    {
        "ALERT: INJURY", "57Y-03CY",
        "[ START OF RECORDING ]\n\n"
        "[XX:05] Sec. Rose: How do we tell the public what occured to Applicant #882?\n\n"
        "[XX:05] Dir. Nexus: Tell them that he was already \"infected\" prior to his injury that resulted in his termination. We cannot afford more people knowing.\n\n"
        "[XX:06] Sec. Rose: ... Okay.\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:22] Sec. Rose: It has come to our attention that an applicant has gotten injured rather badly during a routine monitoring session, as suggested by their work parameters.\n"
        "We at Void.INC are heavily investigating this matter, but it seems that the applicant was \"infected\" prior to this situation. We continue to advise other applicants to\n"
        "continue monitoring the situation with the Star and to avoid and stay away from any potentially lethal and /or harmful objects immediately.\n\n"
        "[ RECORDING PAUSED ]\n\n"
        "[XX:24] Dir. Nexus: Good. Keep the rest of the details under wraps for now. Any news regarding Ryu?\n\n"
        "[XX:25] Sec. Rose: ... Nothing new, no. He actually seems like he's just gone.\n\n"
        "[XX:25] Dir. Nexus: I see.\n\n"
        "[ END OF RECORDING ]"
    },
    {
        "FWD: ALERT: INJURY", "57Y-04CY",
        "[ START OF TRANSMISSION ]\n\n"
        "Eng. Ryu: Do you see how big a joke Corporate is becoming?\n\n"
        "Rschr. Erik: Trying to cover up a situation like that? Seriously?\n\n"
        "Jr. Eng. Pearl: Yeah, it's definitely showing how desperate they're becoming. Not a surprise.\n\n"
        "RA. Tessa: Are you sure it's wise to hold their logs like this? I mean, what if you get caught?\n\n"
        "Eng. Ryu: Then they can try finding me, assuming it doesn't get to them first.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "Eng. Ryu: Scramble transmission keys, my sensors are picking it up.\n\n"
        "[ END OF TRANSMISSION ]"
    },
    {
        "URGENT EMERGENCY #2", "57Y-10CY",
        "[ START OF TRANSMISSION ]\n\n"
        "Eng. Willius: Ryu, we need help. We have an emergency.\n\n"
        "Eng. Ryu: Get to the point.\n\n"
        "Eng. Willius: This... thing... it's controlling our systems. Our current course is to pass by the Star in 'V-1D-10' in 30 cycles.\n\n"
        "Eng. Ryu: I cannot do anything about that. Your shuttle was already destined to never return the moment it left HQ.\n\n"
        "Eng. Willius: ... You... You can't be serious... So what then? Just collect data until we die?\n\n"
        "Eng. Ryu: Correct. The Star in 'V-1D-10' will go hypernova well before you even pass by it. All you can do i-\n\n"
        "Eng. Willius: Hypernova... as in... a collapsar?\n\n"
        "Eng. Ryu: Yes. A collapsar. Even if you turn around now, you are not going to outrun it. Again, all you ca-\n\n"
        "Eng. Willius: No other way out?\n\n"
        "Eng. Ryu: No. All you can do is continue to collect data, unfortunately.\n\n"
        "[ TRANSMISSION ON HOLD ]\n\n"
        "Rschr. Eris: Is there anything you'd specifically need, Ryu?\n\n"
        "Eng. Ryu: Everything you can get. I will continue to push firmware updates to your terminals just to keep you both safe from the psychological dangers.\n\n"
        "Rschr. Eris: Understood... Ryu, will we be remem-\n\n"
        "Eng. Ryu: No. Your data will outlive your memory.\n\n"
        "[ END OF TRANSMISSION ]"
    },
    {
        "TRANSMISSION 5712JTF", "57Y-12CY",
        "[ START OF TRANSMISSION ]\n\n"
        "Eng. Ryu: Ryu to U-02-H. U-01-A is going to hell. I am managing the situation as well as I can; however, I need your status.\n\n"
        "Dir. Newt: It's been a while since we've last heard from you, U-01-A. Nothing out of the ordinary; however, how are you doi-\n\n"
		"Eng. Ryu: I'm not looking for small talk. Give me your full status.\n\n"
        "Dir. Newt: Is something going on in U-01-A?\n\n"
        "Eng. Ryu: Afraid so. Since you aren't making this easy for me, I'll just get to the point: disable your multiversal generators. The links need to be severed.\n\n"
        "Dir. Newt: ... Odd request, to say the least. What's going on over there?\n\n"
        "Eng. Ryu: Infection. Disable everything. I will be containing everything here, even if it means destroying it all in the end.\n\n"
        "Dir. Newt: Understood, U-01-A. We will comply as soon as we are able.\n\n"
        "Eng. Ryu: Good luck. We won't be talking for a long time.\n\n"
		"[ END OF TRANSMISSION ]"
    },
    {
        "SYSTEM COMPROMISED", "57Y-16CY",
		"[ START OF RECORDING ]\n\n"
        "[XX:11] Dir. Nexus: It has come to our attention that the restricted terminal has been added to the main terminal network.\n\n"
        "[XX:11] Board-03: Is that so? Well, it seems our little engineer enjoys causing a little chaos.\n\n"
        "[XX:12] Dir. Nexus: Indeed. Our Omega-level personnel are attempting to bypass these restrictions with no luck. We're all locked out of the system.\n\n"
        "[XX:13] Board-06: And how is it that this engineer has access to these systems in the first place?\n\n"
		"[XX:13] Dir. Nexus: ... Ryu is no ordinary engineer. He was selected specifically to create and maintain our systems. He has the highest clearance possible.\n\n"
		"[XX:14] Board-01: Wonderful. So not only did he create these systems, he clearly had contingencies in place to lock everyone out if he felt like it.\n\n"
        "[XX:15] Dir. Nexus: Unfortunately so. In the meantime, we are sitting ducks. We've already lost full contact with both our R&D and Engineering departments, and we aren't one step\n"
        "closer to figuring this entity out.\n\n"
		"[XX:17] Board-06: What was it that the engineer called it again? 'Archangel'? Why did he choose such a name to designate such a creature?\n\n"
        "[XX:18] Dir. Nexus: I don't know for certain. He submitted the proposal without any background or explanation. For all I know, this entity is capable of infecting biological and\n"
        "technological lifeforms and systems alike. Ryu knew something and purposely kept us in the dark.\n\n"
        "[XX:20] Board-03: Well then, it seems we're at an impasse. It's clear we canno-\n\n"
        "[ ERROR: RECORDING CORRUPTED ]\n\n"
        "[XX:24] Board-01: WHAT IS THAT!?\n\n"
		"[ ERROR: RECORDING CORRUPTED ]\n\n"
        "[XX:27] Board-06: GET ME OUT OF HE-\n\n"
		"[ ERROR: RECORDING CORRUPTED ]\n\n"
		"[XX:29] Board-03: ... It seems... death is inevitable...\n\n"
        "[ ERROR: RECORDING CORRUPTED ]"
    },
    {
        "PROJECT: TERMINAL-02A", "57Y-20CY",
        "[ START OF RECORDING ]\n\n"
        "Dir. Nexus: As you all may be aware, our systems have been compromised and locked by Engineer Ryu. Due to our inability to access these terminals, along with the impending threat...\n"
        "I have decided to lead a child company, 'Void.TD', to create new terminals from scratch to protect our own personnel against the dangers that Ryu had refused to provide context to.\n\n"
		"These terminals will be completely isolated from our current network, and will not have or be connected in any way to Ryu's systems. However, due to the urgency of this situation\n"
		"and our lack of resources, time, engineers, and R&D personnel and departments, we will be outsourcing the creation of these terminals to any willing applicants.\n\n"
        "These terminals are specifically designed to be resistant to any potential influence from the 'Archangel', and will have built-in defensive measures and protocols to ensure user\n"
		"safety. Applicants will be compensated generously for their work, and will be given a high priority clearance to access these new terminals and develop them.\n\n"
		"Time is of the essence, so please apply as soon as you are able. We cannot afford to lose any more personnel to this 'Archangel'.\n\n"
	    "[ END OF RECORDING ]"
    },
    {
        "DESTINY-4514: COLLAPSAR", "57Y-36CY",
        "[ START OF TRANSMISSION ]\n\n"
        "Eng. Willius: Well... I don't know if anyone is out there to hear this, but this is Destiny-4514... about T-5 minutes away from passing according to our onboard sensors...\n\n"
        "Rschr. Eris: We did all we could. The data we've gathered should be enough to aid in future research to... stop whatever it is going on.\n\n"
		"Eng. Willius: Yeah... it's hard to believe that the hypernova already occurred... we're just waiting around to be vaporised...\n\n"
        "Rschr. Eris: At least... we aren't alone. I hope Ryu knows what he's doing. I don't know how valuable our data truly is, but... I hope it helps one way or another.\n\n"
        "Eng. Willius: ... If only we were far enough away to do that... hah...\n\n"
        "Rschr. Eris: What would it be like? Do we even have the time to see it?\n\n"
        "Eng. Willius: We'll... see the collapsar and jet at the same time... I guess...\n\n"
        "Rschr. Eris: ... So... it's instant, then... No suffering, at least...\n\n"
        "Eng. Ryu: Greetings, and goodbye, Destiny-4514. I have received your data, and it is sufficient for my intentions. Thank you for your work, and I bid you both a final farewell.\n\n"
        "Eng. Willius: Surprised you're still as detached as ever.\n\n"
        "Eng. Ryu: Would you rather I mourn? I don't have time for such trivialities, currently. Your data will serve its purpose soon enough.\n\n"
        "...: Impact in T-30 seconds.\n\n"
        "Eng. Willius: Were we really destined to never return from this mission?\n\n"
        "Eng. Ryu: Correct. The moment your shuttle received its designation and was sent to 'V-1D-10' over drones, that was it. You were already officially off the system.\n\n"
        "Eng. Willius: ... I see...\n\n"
		"...: Impact in T-10 seconds.\n\n"
        "Rschr. Eris: Well... goodbye, then...\n\n"
		"...: Impact in T-5 seconds.\n\n"
        "Eng. Willius: Yeah... goodb-\n\n"
        "[ ERROR: TRANSMISSION LOST ]"
    }
};