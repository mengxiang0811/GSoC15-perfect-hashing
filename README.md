# GSoC15-perfect-hashing

Linux XIA needs to hash XID types to unique buckets in a couple of places of its codebase. This mapping must be highly efficient because it affects the speed of routing packets, so if there is a guarantee that no two XID types hash to the same bucket, the code can make fewer memory accesses. This problem is called perfect hashing in the literature. This need of Linux XIA is similar to the one IPv4 has with its protocol field; the equivalent field in IPv6 is the next header field. However, IP's problem is much simpler because its protocol field is only a byte long, whereas an XID type is four bytes long. The currently implemented solution in Linux XIA is limited and will become unwieldy as the number of principals grows. So the goal of this project is to investigate what is the best perfect hashing for mapping XID types, and implement it in Linux XIA.

This project comprises the following steps:

1. Researching perfect hashing algorithms to implement.

2. Implementing an evaluation environment for the chosen algorithms.

3. Implementing the chosen algorithms to run in the evaluation environment.

4. Evaluating all implemented algorithms.

5. (Research credits) Designing and implementing a perfect hashing better than all others already evaluated. If you get this extra item done, your mentor will evaluate if your algorithm is suitable for publication, and work with you to write the paper.

6. Implementing the best algorithm into Linux XIA. Notice that the new algorithm may require a change of interface, which would trigger the need for patches for all implemented principals. But given that the interface is small, and will still be small, these patches should not be a burden. The current interface used by principals includes the following functions: vxt_register_xidty(), vxt_unregister_xidty(), xt_to_vxt(), and xt_to_vxt_rcu().

<b>Allocated mentor</b>: Qiaobin Fu.

<b>GSoC student</b>: Pranav Goswami.

Citation: <a href="Implementing perfect hashing for mapping XID types to loaded principals">Implementing perfect hashing for mapping XID types to loaded principals</a>

<b>Acknowledgement</b>: Thanks for Yijun Zhou's work on implementing the dynamic perfect hashing algorithm, and Rishi Shah's work on implementing the d-left hashing algorithm.
