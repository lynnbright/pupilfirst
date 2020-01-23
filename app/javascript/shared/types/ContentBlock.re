exception UnexpectedBlockType(string);

type markdown = string;
type url = string;
type title = string;
type caption = string;
type embedCode = string;
type filename = string;

type blockType =
  | Markdown(markdown)
  | File(url, title, filename)
  | Image(url, caption)
  | Embed(url, embedCode);

type t = {
  id: string,
  blockType,
  sortIndex: int,
};

let decodeMarkdownContent = json =>
  Json.Decode.(json |> field("markdown", string));
let decodeFileContent = json => Json.Decode.(json |> field("title", string));
let decodeImageContent = json =>
  Json.Decode.(json |> field("caption", string));
let decodeEmbedContent = json =>
  Json.Decode.(
    json |> field("url", string),
    json |> field("embedCode", string),
  );

let decode = json => {
  open Json.Decode;

  let blockType =
    switch (json |> field("blockType", string)) {
    | "markdown" => Markdown(json |> field("content", decodeMarkdownContent))
    | "file" =>
      let title = json |> field("content", decodeFileContent);
      let url = json |> field("fileUrl", string);
      let filename = json |> field("filename", string);
      File(url, title, filename);
    | "image" =>
      let caption = json |> field("content", decodeImageContent);
      let url = json |> field("fileUrl", string);
      Image(url, caption);
    | "embed" =>
      let (url, embedCode) = json |> field("content", decodeEmbedContent);
      Embed(url, embedCode);
    | unknownBlockType => raise(UnexpectedBlockType(unknownBlockType))
    };

  {
    id: json |> field("id", string),
    blockType,
    sortIndex: json |> field("sortIndex", int),
  };
};

let sort = blocks => blocks |> List.sort((x, y) => x.sortIndex - y.sortIndex);
let sortArray = blocks => blocks |> Array.to_list |> sort |> Array.of_list;

let id = t => t.id;
let blockType = t => t.blockType;
let sortIndex = t => t.sortIndex;

let makeMarkdownBlock = markdown => Markdown(markdown);
let makeImageBlock = (fileUrl, caption) => Image(fileUrl, caption);
let makeFileBlock = (fileUrl, title, fileName) =>
  File(fileUrl, title, fileName);
let makeEmbedBlock = (url, embedCode) => Embed(url, embedCode);

let make = (id, blockType, sortIndex) => {id, blockType, sortIndex};

let makeFromJs = js => {
  let id = js##id;
  let sortIndex = js##sortIndex;
  let blockType =
    switch (js##content) {
    | `MarkdownBlock(content) => Markdown(content##markdown)
    | `FileBlock(content) =>
      File(content##url, content##title, content##filename)
    | `ImageBlock(content) => Image(content##url, content##caption)
    | `EmbedBlock(content) => Embed(content##url, content##embedCode)
    };

  make(id, blockType, sortIndex);
};

let blockTypeAsString = blockType =>
  switch (blockType) {
  | Markdown(_markdown) => "markdown"
  | File(_url, _title, _filename) => "file"
  | Image(_url, _caption) => "image"
  | Embed(_url, _embedCode) => "embed"
  };

let incrementSortIndex = t => {...t, sortIndex: t.sortIndex + 1};

module Fragments = [%graphql
  {|
  fragment allFields on ContentBlock {
    id
    blockType
    sortIndex
    content {
      ... on ImageBlock {
        caption
        url
        filename
      }
      ... on FileBlock {
        title
        url
        filename
      }
      ... on MarkdownBlock {
        markdown
      }
      ... on EmbedBlock {
        url
        embedCode
      }
    }
  }
|}
];

module Query = [%graphql
  {|
    query($targetId: ID!, $versionOn: Date) {
      contentBlocks(targetId: $targetId, versionOn: $versionOn) {
        id
        blockType
        sortIndex
        content {
          ... on ImageBlock {
            caption
            url
            filename
          }
          ... on FileBlock {
            title
            url
            filename
          }
          ... on MarkdownBlock {
            markdown
          }
          ... on EmbedBlock {
            url
            embedCode
          }
        }
      }
      versions(targetId: $targetId)
  }
|}
];
