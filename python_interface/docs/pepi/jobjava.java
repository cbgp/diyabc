TaskFlowJob job = new TaskFlowJob();
job.setName("job name");
job.setPriority(JobPriority.NORMAL);
job.setCancelJobOnError(false);
job.setDescription("Mon job");
job.addGenericInformation("param1","val1");
job.addGenericInformation("param2","val2");
JobEnvironment je = new JobEnvironment();
je.setJobClasspath(new String[]{"/classpath/","/classpath2/"});
job.setEnv(je);
            
SchedulerAuthenticationInterface auth = 
    SchedulerConnection.waitAndJoin("protocol://host:port");

Scheduler scheduler = null;
scheduler = auth.login(Credentials.getCredentials());
PublicKey pubKey = auth.getPublicKey();
scheduler = auth.login(
        Credentials.createCredentials(new CredData("demo", "demo"), pubKey));
JobId myJobId = scheduler.submit(job);
JobResult myResult = scheduler.getJobResult(myJobId);
