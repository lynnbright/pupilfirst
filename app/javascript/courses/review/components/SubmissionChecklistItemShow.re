[@bs.config {jsx: 3}];

module ChecklistItem = SubmissionChecklistItem;

let str = React.string;

let kindIconClasses = result => {
  switch ((result: ChecklistItem.result)) {
  | ShortText(_text) => "if i-short-text-regular md:text-base text-gray-800 if-fw"
  | LongText(_markdown) => "if i-long-text-regular md:text-base text-gray-800 if-fw"
  | Link(_link) => "if i-link-regular md:text-base text-gray-800 if-fw"
  | MultiChoice(_text) => "if i-check-circle-alt-regular md:text-base text-gray-800 if-fw"
  | Files(_attachments) => "if i-file-regular md:text-base text-gray-800 if-fw"
  };
};

let showFiles = attachments => {
  <div className="flex flex-wrap">
    {attachments
     |> Array.map(attachment => {
          <a
            key={"file-" ++ (attachment |> ChecklistItem.attachmentUrl)}
            href={attachment |> ChecklistItem.attachmentUrl}
            target="_blank"
            className="mt-1 mr-3 flex border overflow-hidden rounded hover:shadow-md border-pink-400 bg-white text-pink-700 hover:border-pink-600 hover:text-pink-700">
            <span
              className="course-show-attachments__attachment-title rounded text-xs font-semibold inline-block whitespace-normal truncate w-32 md:w-42 h-full px-3 py-2 leading-loose">
              {attachment |> ChecklistItem.attachmentName |> str}
            </span>
            <span
              className="flex w-10 justify-center items-center p-2 bg-pink-700 text-white">
              <PfIcon className="if i-download-regular" />
            </span>
          </a>
        })
     |> React.array}
  </div>;
};

let showlink = link =>
  <a
    href=link
    target="_blank"
    className="max-w-fc mt-1 mr-3 flex border overflow-hidden rounded hover:shadow-md border-indigo-400 bg-white text-indigo-700 hover:border-blue-600 hover:text-indigo-800">
    <span
      className="course-show-attachments__attachment-title rounded text-xs font-semibold inline-block whitespace-normal truncate w-32 md:w-42 h-full px-3 py-2 leading-loose">
      {link |> str}
    </span>
    <span
      className="flex w-10 justify-center items-center p-2 bg-indigo-700 text-white">
      <PfIcon className="if i-external-link-regular" />
    </span>
  </a>;

let statusIcon = (updateChecklistCB, status) => {
  switch (updateChecklistCB, status: ChecklistItem.status) {
  | (None, Passed) =>
    <FaIcon
      classes="fas fa-check-circle text-green-500 text-lg mr-3 -ml-6 mt-1"
    />
  | (None, Failed) =>
    <FaIcon
      classes="fas fa-times-circle text-red-500 text-lg mr-3 -ml-6 mt-1"
    />
  | (_, _) => React.null
  };
};

let showStatus = status => {
  switch ((status: ChecklistItem.status)) {
  | Passed =>
    <div className="bg-green-200 rounded px-1 py-px text-green-800 text-tiny">
      {"Passed" |> str}
    </div>
  | Failed =>
    <div className="bg-red-200 rounded px-1 py-px text-red-800 text-tiny">
      {"Failed" |> str}
    </div>
  | NoAnswer => React.null
  };
};

let statusButtonSelectedClasses = (status, currentStatus) => {
  "inline-flex items-center cursor-pointer leading-tight px-3 py-2 font-semibold inline-block text-xs relative hover:bg-gray-100 hover:text-gray-700 "
  ++ (
    switch (currentStatus: ChecklistItem.status, status: ChecklistItem.status) {
    | (Passed, Passed) => "bg-green-100 hover:bg-green-100 text-green-800 hover:text-green-800 border-green-500 z-10"
    | (Failed, Failed) => "bg-red-100 hover:bg-red-100 text-red-800 hover:text-red-800 border-red-500 z-10"
    | (_, _) => "bg-white"
    }
  );
};

let statusButtons = (index, status, callback, checklist) =>
  <div className="mt-2">
    <div
      onClick={_ => callback(checklist |> ChecklistItem.makeFailed(index))}
      className={
        "border border-gray-500 rounded -ml-px "
        ++ statusButtonSelectedClasses(ChecklistItem.Failed, status)
      }>
      <PfIcon className="if i-times-light if-fw mr-2" />
      {"This has a problem" |> str}
    </div>
    {switch ((status: ChecklistItem.status)) {
     | NoAnswer => React.null
     | Passed
     | Failed =>
       <div
         onClick={_ =>
           callback(checklist |> ChecklistItem.makeNoAnswer(index))
         }
         className={statusButtonSelectedClasses(
           ChecklistItem.NoAnswer,
           status,
         )}>
         <i className="fas fa-redo" />
       </div>
     }}
  </div>;

let computeShowResult = (pending, checklistItem) =>
  switch (pending, checklistItem |> ChecklistItem.status) {
  | (true, NoAnswer | Passed | Failed) => true
  | (false, Failed) => true
  | (false, NoAnswer | Passed) => false
  };

let cardClasses = pending => {
  pending ? "mt-3" : "rounded shadow mt-4 ";
};

let cardHeaderClasses = pending => {
  "text-sm font-semibold flex items-center justify-between "
  ++ (pending ? "" : "p-4 bg-white rounded cursor-pointer");
};

let cardBodyClasses = pending => {
  "pl-5 md:pl-7 p-3 pb-4 " ++ (pending ? "" : "border-t bg-gray-200 rounded-b");
};

[@react.component]
let make = (~index, ~checklistItem, ~updateChecklistCB, ~checklist, ~pending) => {
  let (showResult, setShowResult) =
    React.useState(() => computeShowResult(pending, checklistItem));

  React.useEffect1(
    () => {
      let newShowResult = computeShowResult(pending, checklistItem);
      newShowResult == showResult ? () : setShowResult(_ => newShowResult);
      None;
    },
    [|updateChecklistCB|],
  );
  let status = checklistItem |> ChecklistItem.status;

  <div className={cardClasses(pending)}>
    <div
      onClick={_ => setShowResult(_ => true)}
      className={cardHeaderClasses(pending)}>
      <div className="inline-flex items-center">
        {statusIcon(updateChecklistCB, status)}
        <PfIcon
          className={kindIconClasses(checklistItem |> ChecklistItem.result)}
        />
        <p className="pl-2 tracking-wide">
          {checklistItem |> ChecklistItem.title |> str}
        </p>
      </div>
      <div className="inline-block">
        {showResult
           ? showStatus(status)
           : <button> <i className="fas fa-chevron-down" /> </button>}
      </div>
    </div>
    {showResult
       ? <div className={cardBodyClasses(pending)}>
           <div>
             {switch (checklistItem |> ChecklistItem.result) {
              | ShortText(text) => <div> {text |> str} </div>
              | LongText(markdown) =>
                <MarkdownBlock profile=Markdown.Permissive markdown />
              | Link(link) => showlink(link)
              | MultiChoice(text) => <div> {text |> str} </div>
              | Files(attachments) => showFiles(attachments)
              }}
           </div>
           {switch (updateChecklistCB) {
            | Some(callback) =>
              statusButtons(index, status, callback, checklist)
            | None => React.null
            }}
         </div>
       : React.null}
  </div>;
};
