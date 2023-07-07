# Outline for *Mandalin*
## Introduction
 *Mandalin* is a sort of blend of 4X and grand strategy games
 with a strong emphasis on simulation which aims to provide
 personal yet sweeping narratives about history, language,
 culture, and economics. It takes place on a hex-based grid
 tiled over a spherical world with randomly generated continents,
 biomes, and various features which inform the evolution of
 peoples and cultures across the globe.

 We want to avoid creating another game in which the central
 goal is to craft the most efficient engine of production,
 the pitfall of most 4X games. If a player wants to command a few
 steppe nomads on the fringes of a larger empire, we want them
 to be able to do so, to enjoy doing so. Of course, this shouldn't
 come at the expense of the enjoyment of those who wish to play
 expansive empires. In *Mandalin*, it will just be much harder to
 build China or Rome. Furthermore, we will be motivated by a
 certain mantra imported from the *Dwarf Fortress* community:
 losing should be fun.

## Philosophy
 *Mandalin* is first and foremost intended as a simulator
 of history. This is an impossible task.
 We must balance complexity with readability; that is, the
 game cannot be so inscrutable that a hypothetical player
 cannot determine the state of even a part of the world at
 any given point in time, but we also want it to be complex
 and fluid enough that it can entertain said player for
 a great many hours. In short, there needs to be a common
 language across the various social, political, religious,
 economic, and linguistic systems that might arise.

 One of the primary goals of *Mandalin* is to somewhat
 accurately represent the diverse systems of power that
 have developed over the history of the real world, and
 we want to do this as organically as possible. We do
 not want to define a set number of systems which all
 nations and states must adhere to; we want the course
 of history and the choices of historical actors to
 naturally give rise to systems that entrench themselves
 in particular national, ethnic, religious, and linguistic
 circumstances. In short, we want to be able to simulate
 the clan politics of the Scottish Highlands as well as
 the shogunate of feudal Japan using the same set of
 fundamental rules.

 The initial impetus for this whole project was my desire
 to simulate the complex sociopolitical systems of
 pre-modern Southeast Asia which don't fit neatly into
 the very European, very feudal structures often hard-coded
 into other grand strategy games. One popular way of
 understanding these systems is as a set of overlapping
 mandalas. States, anchored in particular cities or regions,
 exerted influence over an area that often overlapped with
 other states, other cities, and there were no clear
 boundaries where it could be said that one state's influence
 ended and another's began.

 This contrasts with the traditional depiction of feudal
 Europe, wherein states are composed of a clear locus
 of power which presides over a set number of holdings,
 governed by nobles who owe allegiance to the singular
 throne and whose financial and military responsibilities to
 the state are clearly defined. These states have definite
 borders which often align clearly with ethnic, religious,
 and linguistic borders. The influence of the state ends
 where another's begins. This of course is not accurate to
 how actual feudal Europe worked, but it is a nice abstraction
 which allows these simulations to place a certain limit of
 the maximum complexity of their systems. Anyone trying to
 understand the state of the world at any given point has only
 to look at the lines drawn on the map.

 This is not entirely true for many games, but it goes a long
 way towards defining the kind of simulation we hope to contrast
 ourselves against. *Mandalin* should be able to seamlessly
 transition between nebulous simulations of power in highly
 decentralized or pluricentric systems to extremely hierarchical,
 stringently defined nations where influence is cleanly
 divided and the responsibility of vassals laid in stone.

 We are going to achieve this by allowing systems to naturally
 float from one end of the spectrum to the next, with natural
 tendencies for nations to head towards solidification.

 In *Mandalin*, states are defined by loci, often cities or
 military fortifications, which exert some amount of influence
 over surrounding lands—the degree of which is determined by
 the economic, military, and cultural power of the particular
 loci and by extension of the state. This influence is nebulous
 at first, freely overlapping with other loci and other states,
 until one loci whether through an appeal to the head-of-state
 or by military campaign determines to press its claim to a
 particular area, thereby seizing sole official control. While
 the influence of other loci still freely flows over this
 now-concrete border, it limits the ability of other loci to
 demand taxes or levies from this area. Thus, from a
 free-flowing field of influence we see arise cleanly divided
 provinces, official jurisdictions, wherein a single locus
 reigns. Of course, the influence of other loci or states is
 still felt across these borders, swaying the cultural,
 linguistic, and religious developments within particular
 regions.

 If a loci becomes more powerful than the official capital
 of its state, it may decide to split off or to usurp control;
 if a loci is more strongly aligned with a neighboring state,
 it may decide to change hands; if the influence of a loci
 evaporates, it may dissolve into smaller polities or
 disappear altogether, lost into sub-state systems.

 It is tempting to abstract history into the rise and fall
 of states, but this is to neglect the modes of social
 organization that have defined human life for most of our
 existence. We have wandered the world for hundreds of
 thousands of years, but it is only for the last five, give
 or take, that we have lived under states. In some sense,
 sub-state systems are more difficult to simulate; yet, it
 is not as though sub-state and state systems exist discretely;
 they overlap, they interact, their boundaries are not so
 neat as we might like. Thus, we must define a set of rules
 by which substate systems operate both in states and in
 stateless societies; in addition, sub-states and states must
 exist along a spectrum. There will be two main forms of
 sub-state systems: identities and societies.

 Identities exist largely parallel to states. They take
 two primary forms: elementary and synthetic. Elementary
 identities include linguistic, religious, and regional
 identities which combine together to form synthetic
 identities, what we often call ethnic or cultural
 identities; for example, a peoples who exist across a
 contiguous space whose languages are sufficiently similar
 and who all practice the same faith are likely to form some
 collective identity, albeit a weaker one than those who
 just share the same religion or speak the same language.

 Each identity is spread over a certain area (containing
 certain hexes) and has on each hex an associated power,
 indicating how strongly this identity is felt by those
 living there, and a percentage, indicating the percentage
 of the population which holds this identity. The
 percentages of all the identities in a hex do not
 necessarily add up to one hundred percent, even if they
 are of the same type. That is, there may exist a linguistic
 identity A and B which both include 60% of a particular
 hex; this just means that some number of the people in this
 hex are bilingual. Similarly, people are perfectly capable
 of holding two distinct religious identities (take Japan
 as an example, where Shinto and Buddhism coexist, albeit
 often forming a sort of synthetic identity between
 themselves). Similarly, two regional identities may
 overlap or one may contain another; the people of
 Sussex can identify with Sussex just as they identify with
 England more broadly.

 Societies, on the other hand, are a bit more complex,
 dictating, in part, the way that populations move and
 how they extract resources from the land. Each society
 has a "way of life" which is classified as hunter-gatherer,
 pastoralist, peripatetic, or agriculturalist. These
 determine, in part, the carrying capacity of the land,
 influencing how populations grow across certain hexes.
 In addition, societies can be settled, partly nomadic, or
 nomadic, affecting how they move around a given territory.
